#include "ClientWorldManager.h"
#include "Networking/Client/Client.h"
#include "Utils/Input.h"
#include "Utils/NetUtils.h"
#include "Networking/Packet/Instances/Player/PlayerAttack.h"
#include "Networking/Packet/Instances/PlayerInput.h"
#include "Networking/Packet/Instances/Item/ItemDiscard.h"
#include "Libraries/raylib-imgui-compat/rlImGui.h"

ClientWorldManager::ClientWorldManager(std::shared_ptr<Client> client)
    : client(client), last_input_send_time(std::chrono::steady_clock::now()) {
    
    // Initialize camera
    camera.position = {0.0f, 10.0f, 10.0f};
    camera.target = {0.0f, 0.0f, 0.0f};
    camera.up = {0.0f, 0.0f, -1.0f};
    camera.fovy = 70.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void ClientWorldManager::update(float delta_time) {

    // Process local player input ONLY IF player exists and is alive
    if (client->peers.local_server_side_id != 0 && get_local_player() && !get_local_player()->is_dead()) {
        process_local_player_input(delta_time);
        
        // Send input to server if enough time has passed
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_input_send_time);
        
        if (elapsed.count() >= input_send_interval && has_local_player_moved()) {
            send_local_player_input();
            last_input_send_time = now;
        }
    }
    
    // Check for inventory toggle
    if (Input::is_key_pressed(KEY_E)) {
        toggle_inventory();
    }
    
    // Update camera to follow local player
    update_camera(delta_time);

    // Lock for enemy tick and physics update
    {
        std::lock_guard<std::mutex> lock(world_state_mutex);
        
        // Tick enemies
        for (auto& [enemy_id, enemy] : enemies) {
            // Gather pointers to all players for enemy AI
            std::vector<Player*> player_ptrs;
            for (auto& [peer_id, player] : players) {
                player_ptrs.push_back(&player);
            }
            enemy.tick(delta_time, player_ptrs);
        }
        
        // Apply physics (collision checking)
        PhysicsManager::update(&players, &enemies, &objects, nullptr, this);
    }
}

void ClientWorldManager::draw_3d() {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    // Draw all players
    for (auto& [peer_id, player] : players) {
        player.draw3D(camera);
        
        // Draw attack range circle if player is attacking (filled disc)
        if (player.attacking) {
            raylib::Vector3 player_pos = player.transform.get_position();
            // Use a very short cylinder to create a filled disc on the ground
            DrawCylinder(player_pos, player.range, player.range, 0.05f, 32, ColorAlpha(RED, 0.5f));
        }
    }
    
    // Draw all enemies (avoid structured binding in case of map corruption)
    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        Enemy& enemy = it->second;
        enemy.draw3D(camera);
    }
    
    // Draw all objects
    for (auto& [object_id, object] : objects) {
        object.Draw(camera);
    }
}

void ClientWorldManager::draw_2d() {
    // Draw player UI elements (names, health bars)
    for (auto& [peer_id, player] : players) {
        player.draw2D(camera);
    }
    
    // Draw enemy UI elements (health bars)
    for (auto& [enemy_id, enemy] : enemies) {
        enemy.draw2D(camera);
    }
    
    // Draw inventory UI
    draw_inventory_ui();
}

void ClientWorldManager::clear() {
    players.clear();
    objects.clear();
    enemies.clear();
    items.clear();
    show_inventory = false;
}



Player* ClientWorldManager::get_local_player() {
    return get_player(client->peers.local_server_side_id);
}



void ClientWorldManager::add_player(const Player& player) {
    players[player.id] = player;
    if (player.id == client->peers.local_server_side_id) {
        players[player.id].is_local = true;
        // Initialize last_sent_transform when local player is added
        last_sent_transform = players[player.id].transform;
    }
}

void ClientWorldManager::remove_player(uint32_t peer_id) {
    players.erase(peer_id);
}

void ClientWorldManager::update_player(uint32_t peer_id, 
    const ObjectTransform& transform, float health, float damage, 
    float max_health, float range, float speed, uint64_t attack_cooldown, uint64_t last_attack_time, bool attacking, const Inventory& inventory) {

    auto it = players.find(peer_id);
    if (it != players.end()) {
        // Don't update local player's transform from server, as it causes jittering
        if (peer_id != client->peers.local_server_side_id) {
            it->second.transform = transform;
        }
        it->second.health = health;
        it->second.damage = damage;
        it->second.max_health = max_health;
        it->second.range = range;
        it->second.speed = speed;
        it->second.attack_cooldown = attack_cooldown;
        it->second.last_attack_time = last_attack_time;
        it->second.attacking = attacking;
        it->second.inventory = inventory;
    }
}

Player* ClientWorldManager::get_player(uint32_t peer_id) {
    auto it = players.find(peer_id);
    return (it != players.end()) ? &it->second : nullptr;
}



void ClientWorldManager::add_object(const Object& object) {
    objects[object.id] = object;
}

void ClientWorldManager::remove_object(uint32_t object_id) {
    objects.erase(object_id);
}

Object* ClientWorldManager::get_object(uint32_t object_id) {
auto it = objects.find(object_id);
    return (it != objects.end()) ? &it->second : nullptr;
}

void ClientWorldManager::add_enemy(const Enemy& enemy) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    enemies[enemy.id] = enemy;
    
}

void ClientWorldManager::remove_enemy(uint32_t enemy_id) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    enemies.erase(enemy_id);
}

void ClientWorldManager::update_enemy(uint32_t enemy_id, const ObjectTransform& transform, float health) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    auto it = enemies.find(enemy_id);
    if (it != enemies.end()) {
        it->second.transform = transform;
        it->second.health = health;
    }
    else {
        TRACE("Updating an enemy that does not exist!");
    }
}

Enemy* ClientWorldManager::get_enemy(uint32_t enemy_id) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    auto it = enemies.find(enemy_id);
    return (it != enemies.end()) ? &it->second : nullptr;
}



void ClientWorldManager::apply_world_snapshot(const WorldSnapshotPacket& snapshot) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    clear();
    
    // Load all players
    for (const auto& [peer_id, player] : snapshot.players) {
        players[peer_id] = player;
        if (peer_id == client->peers.local_server_side_id) {
            players[peer_id].is_local = true;
            // Initialize last_sent_transform when local player is first loaded
            last_sent_transform = players[peer_id].transform;
        }
    }
    
    // Load all objects
    for (const auto& [object_id, object] : snapshot.objects) {
        objects[object_id] = object;
    }

    // Load all enemies
    for (const auto& [enemy_id, enemy] : snapshot.enemies) {
        enemies[enemy_id] = enemy;
    }
}

void ClientWorldManager::apply_player_updates(const std::vector<PlayerUpdateData>& updates) {
    for (const auto& update : updates) {
        update_player(update.id, update.transform, update.health,
            update.damage, update.max_health, update.range, update.speed, update.attack_cooldown,
            update.last_attack_time, update.attacking, update.inventory);
    }
}

void ClientWorldManager::apply_enemy_updates(const std::vector<EnemyUpdateData>& updates) {
    for (const auto& update : updates) {
        update_enemy(update.id, update.transform, update.health);
    }
}

void ClientWorldManager::send_local_player_input() {
    Player* local_player = get_local_player();
    if (!local_player) return;
    
    PlayerInputPacket packet(local_player->transform);
    client->send_packet(packet);
    
    // Cache the sent transform
    try {
        last_sent_transform = local_player->transform;
    } catch (const std::exception& e) {
        // Log the error but don't crash - the transform might be corrupted
        TRACE("Warning: Failed to cache player transform: %s", e.what());
    }
}



void ClientWorldManager::update_camera(float delta_time) {
    Player* local_player = get_local_player();
    if (!local_player) return;
    
    // Simple third-person camera following the player
    raylib::Vector3 player_pos = local_player->transform.get_position();
    raylib::Vector3 offset = {0.0f, 10.0f, 0.0f};  // Above
    
    camera.target = player_pos;
    camera.position = player_pos + offset;
}



void ClientWorldManager::process_local_player_input(float delta_time) {
    Player* local_player = get_local_player();
    if (!local_player) return;
    
    uint64_t current_time = NetUtils::get_current_time_millis();
    
    // Reset attacking flag after 200ms for visual feedback
    if (local_player->attacking && (current_time - local_player->last_attack_time) > 200) {
        local_player->attacking = false;
    }
    
    // Get input
    raylib::Vector3 movement = {0.0f, 0.0f, 0.0f};
    
    if (Input::is_key_down(KEY_W)) movement.z -= 1.0f;
    if (Input::is_key_down(KEY_S)) movement.z += 1.0f;
    if (Input::is_key_down(KEY_A)) movement.x -= 1.0f;
    if (Input::is_key_down(KEY_D)) movement.x += 1.0f;
    
    // Normalize and apply speed
    if (movement.x != 0.0f || movement.z != 0.0f) {    
        // Apply movement
        local_player->move(movement.Normalize() * local_player->speed * delta_time);
    }

    // Check for player attack input
    if (Input::is_key_pressed(KEY_SPACE)) {
        // Send attack packet if cooldown is ready
        if ((current_time - local_player->last_attack_time) >= local_player->attack_cooldown) {
            local_player->attacking = true;
            local_player->last_attack_time = current_time;
            PlayerAttackPacket attack_packet(local_player->id);
            client->send_packet(attack_packet);
        }
	}
}

bool ClientWorldManager::has_local_player_moved() const {
    Player* local_player = const_cast<ClientWorldManager*>(this)->get_local_player();
    if (!local_player) return false;
    
    // Check if position has changed significantly
    raylib::Vector3 current_pos = local_player->transform.get_position();
    raylib::Vector3 last_pos = last_sent_transform.get_position();
    
    float distance = Vector3Distance(current_pos, last_pos);
    return distance > 0.01f;  // Threshold to avoid sending tiny movements
}

void ClientWorldManager::handle_item_pickup(uint32_t player_id, const Item& item) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    // Store item in client registry
    items[item.id] = item;
    
    // Get player
    Player* player = get_player(player_id);
    if (!player) return;
    
    // Add to inventory
    player->inventory.add_item(item.id);
    
    // Apply effects
    player->apply_item_effects(item.effects);
    
    INFO("CLIENT: Player " + std::to_string(player_id) + 
         " picked up item: " + item.item_name);
}

void ClientWorldManager::request_item_discard(uint32_t item_id) {
    ItemDiscardPacket packet(item_id);
    client->send_packet(packet);
    
    INFO("CLIENT: Requested discard of item: " + std::to_string(item_id));
}

void ClientWorldManager::toggle_inventory() {
    show_inventory = !show_inventory;
}

Item* ClientWorldManager::get_item(uint32_t item_id) {
    auto it = items.find(item_id);
    return (it != items.end()) ? &it->second : nullptr;
}

void ClientWorldManager::draw_inventory_ui() {
    if (!show_inventory) return;
    
    Player* local_player = get_local_player();
    if (!local_player) return;
    
    // Set window size and styling
    ImGui::SetNextWindowSize(ImVec2(650, 750), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    
    ImGui::Begin("Inventory", &show_inventory);
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press E to close");
    ImGui::Separator();
    ImGui::Spacing();
    
    // ===== PLAYER STATS PANEL =====
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.5f, 0.8f, 1.0f)); // Blue border
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.2f, 0.25f, 0.9f)); // Darker background
    
    ImGui::BeginChild("PlayerStats", ImVec2(0, 180), true);
    
    // Header
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Current Stats");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Display stats in two columns
    ImGui::Columns(2, "StatsColumns", false);
    
    // Left column
    ImGui::Text("Health:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "%.0f / %.0f", local_player->health, local_player->max_health);
    
    ImGui::Text("Damage:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "%.0f", local_player->damage);
    
    ImGui::Text("Speed:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.9f, 1.0f), "%.0f", local_player->speed);
    
    // Right column
    ImGui::NextColumn();
    
    ImGui::Text("Range:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.3f, 1.0f), "%.0f", local_player->range);
    
    ImGui::Text("Cooldown:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "%llu ms", local_player->attack_cooldown);
    
    ImGui::Text("Items:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%zu", local_player->inventory.item_ids.size());
    
    ImGui::Columns(1);
    
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // ===== ITEMS SECTION =====
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Items:");
    ImGui::Spacing();
    
    if (local_player->inventory.item_ids.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No items in inventory");
    } else {
        for (uint32_t item_id : local_player->inventory.item_ids) {
            Item* item = get_item(item_id);
            if (!item) continue;
            
            // Create a bordered child window for each item
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            
            std::string child_id = "item_" + std::to_string(item_id);
            ImGui::BeginChild(child_id.c_str(), ImVec2(0, 180), true);
            
            // Left side: Image
            ImGui::BeginGroup();
            
            try {
                const AssetImage& asset = AssetMap::get_image(item->asset_id);
                Texture2D tex = asset.texture;
                rlImGuiImageSize(&tex, 80, 80);
            } catch (...) {
                // If image not found, show placeholder
                ImGui::Dummy(ImVec2(80, 80));
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 40);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 35);
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[IMG]");
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 35);
            }
            
            ImGui::EndGroup();
            
            ImGui::SameLine();
            
            // Right side: Item info and stats
            ImGui::BeginGroup();
            
            // Item name with larger font
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Use default font (can be made larger)
            ImGui::TextWrapped("%s", item->item_name.c_str());
            ImGui::PopFont();
            
            ImGui::Spacing();
            
            // Stats section
            const ItemEffects& e = item->effects;
            bool has_stats = false;
            
            // Helper lambda for colored stat display
            auto display_stat = [&](const char* label, int value, const char* suffix = "") {
                if (value == 0) return;
                has_stats = true;
                ImVec4 color = value > 0 ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
                ImGui::TextColored(color, "  %s: %+d%s", label, value, suffix);
            };
            
            auto display_stat_float = [&](const char* label, float value, const char* suffix = "") {
                if (value == 0.0f) return;
                has_stats = true;
                ImVec4 color = value > 0 ? ImVec4(0.2f, 0.8f, 0.2f, 1.0f) : ImVec4(0.9f, 0.2f, 0.2f, 1.0f);
                ImGui::TextColored(color, "  %s: %+.1f%s", label, value, suffix);
            };
            
            // Display all stats with color coding
            if (e.max_health_boost != 0) 
                display_stat("Max Health", e.max_health_boost);
            if (e.max_health_percentage_boost != 0) 
                display_stat_float("Max Health", e.max_health_percentage_boost * 100, "%");
            if (e.damage_boost != 0) 
                display_stat("Damage", e.damage_boost);
            if (e.damage_percentage_boost != 0) 
                display_stat_float("Damage", e.damage_percentage_boost * 100, "%");
            if (e.speed_boost != 0) 
                display_stat("Speed", e.speed_boost);
            if (e.speed_percentage_boost != 0) 
                display_stat_float("Speed", e.speed_percentage_boost * 100, "%");
            if (e.range_boost != 0) 
                display_stat("Range", e.range_boost);
            if (e.range_percentage_boost != 0) 
                display_stat_float("Range", e.range_percentage_boost * 100, "%");
            if (e.atk_cooldown_reduction != 0) 
                display_stat("Attack Speed", e.atk_cooldown_reduction, " ms");
            if (e.atk_cooldown_percent_reduction != 0) 
                display_stat_float("Attack Speed", e.atk_cooldown_percent_reduction * 100, "%");
            if (e.healing != 0)
                display_stat("Healing", e.healing);
			if (e.healing_percentage != 0) 
                display_stat_float("Healing", e.healing_percentage * 100, "%");
            
            ImGui::EndGroup();
            
            // Discard button at bottom right
            ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 35);
            ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 120);
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
            
            if (ImGui::Button(("Discard##" + std::to_string(item_id)).c_str(), ImVec2(100, 25))) {
                request_item_discard(item_id);
            }
            
            ImGui::PopStyleColor(3);
            
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
            
            ImGui::Spacing();
        }
    }
    
    ImGui::End();
    ImGui::PopStyleVar(2);
}
