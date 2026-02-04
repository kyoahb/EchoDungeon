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
            
            // Only update attacking state for non-local players
            // to avoid flickering
            it->second.attacking = attacking;

        }
        it->second.health = health;
        it->second.damage = damage;
        it->second.max_health = max_health;
        it->second.range = range;
        it->second.speed = speed;
        it->second.attack_cooldown = attack_cooldown;
        it->second.last_attack_time = last_attack_time;
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
    
    // Set window size - wider but shorter
    ImGui::SetNextWindowSize(ImVec2(1000, 500), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
    
    ImGui::Begin("Inventory", &show_inventory);
    
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press E to close");
    ImGui::Separator();
    ImGui::Spacing();
    
    // ===== HORIZONTAL LAYOUT: Stats on left, Items on right =====
    
    // Left side: Player attributes (narrower)
    ImGui::BeginChild("LeftPanel", ImVec2(280, 0), false);
    
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.5f, 0.8f, 1.0f)); // Blue border
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.15f, 0.2f, 0.25f, 0.9f)); // Darker background
    
    ImGui::BeginChild("PlayerStats", ImVec2(0, 0), true);
    
    // Header
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "Attributes");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Display stats in single column with more padding
    ImGui::Text("Health:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.9f, 0.3f, 0.3f, 1.0f), "%.1f / %.1f", local_player->health, local_player->max_health);
    
    ImGui::Text("Damage:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.2f, 1.0f), "%.1f", local_player->damage);
    
    ImGui::Text("Speed:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.3f, 0.9f, 0.9f, 1.0f), "%.1f", local_player->speed);
    
    ImGui::Text("Range:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.3f, 1.0f), "%.1f", local_player->range);
    
    ImGui::Text("Cooldown:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.7f, 0.5f, 1.0f, 1.0f), "%llu ms", local_player->attack_cooldown);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Items:");
    ImGui::SameLine(120);
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "%zu", local_player->inventory.item_ids.size());
    
    ImGui::EndChild();
    ImGui::PopStyleColor(2);
    ImGui::PopStyleVar(2);
    
    ImGui::EndChild(); // End LeftPanel
    
    ImGui::SameLine();
    
    // Right side: Items list (wider, scrollable)
    ImGui::BeginChild("RightPanel", ImVec2(0, 0), false);
    
    ImGui::TextColored(ImVec4(0.8f, 0.8f, 0.8f, 1.0f), "Items");
    ImGui::Separator();
    ImGui::Spacing();
    
    // Scrollable items region
    ImGui::BeginChild("ItemsScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    
    if (local_player->inventory.item_ids.empty()) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "No items in inventory");
    } else {
        for (uint32_t item_id : local_player->inventory.item_ids) {
            Item* item = get_item(item_id);
            if (!item) continue;
            
            // Count non-zero stats to determine card height
            const ItemEffects& e = item->effects;
            int stat_count = 0;
            if (e.max_health_boost != 0) stat_count++;
            if (e.max_health_percentage_boost != 0) stat_count++;
            if (e.damage_boost != 0) stat_count++;
            if (e.damage_percentage_boost != 0) stat_count++;
            if (e.speed_boost != 0) stat_count++;
            if (e.speed_percentage_boost != 0) stat_count++;
            if (e.range_boost != 0) stat_count++;
            if (e.range_percentage_boost != 0) stat_count++;
            if (e.atk_cooldown_reduction != 0) stat_count++;
            if (e.atk_cooldown_percent_reduction != 0) stat_count++;
            if (e.healing != 0) stat_count++;
            if (e.healing_percentage != 0) stat_count++;
            
            // Calculate dynamic height: base height + stat rows (accounting for 2 columns)
            int stat_rows = (stat_count + 1) / 2; // Round up for 2-column layout
            float item_height = 90.0f + (stat_rows * 14.0f); // Base + rows with tight spacing
            item_height = max(item_height, 105.0f); // Minimum height
            
            // Compact item card with dynamic height (NO scrollbars on individual items)
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
            
            std::string child_id = "item_" + std::to_string(item_id);
            ImGui::BeginChild(child_id.c_str(), ImVec2(0, item_height), false); // false = no scrollbar
            
            // Item name at the top
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
            ImGui::TextWrapped("%s", item->item_name.c_str());
            ImGui::PopFont();
            ImGui::Separator();
            
            // Image on the left, stats in columns on the right
            ImGui::BeginGroup();
            
            try {
                const AssetImage& asset = AssetMap::get_image(item->asset_id);
                Texture2D tex = asset.texture;
                rlImGuiImageSize(&tex, 60, 60);
            } catch (...) {
                // If image not found, show placeholder
                ImGui::Dummy(ImVec2(60, 60));
                ImGui::SameLine();
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 30);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[IMG]");
                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 25);
            }
            
            ImGui::EndGroup();
            
            ImGui::SameLine();
            
            // Stats section - use simple vertical list that wraps to 2 columns
            ImGui::BeginGroup();
            
            // Use smaller font and tighter spacing for stats
            ImGui::SetWindowFontScale(0.85f); // Make stats font smaller
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 1));
            
            // Collect all stats first, then display in two columns
            struct StatEntry { const char* label; char value[32]; };
            std::vector<StatEntry> stats;
            
            if (e.max_health_boost != 0) {
                StatEntry s; s.label = "Max Health"; 
                snprintf(s.value, sizeof(s.value), "%+d", e.max_health_boost);
                stats.push_back(s);
            }
            if (e.max_health_percentage_boost != 0) {
                StatEntry s; s.label = "Max Health"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.max_health_percentage_boost * 100);
                stats.push_back(s);
            }
            if (e.damage_boost != 0) {
                StatEntry s; s.label = "Damage"; 
                snprintf(s.value, sizeof(s.value), "%+.1f", e.damage_boost);
                stats.push_back(s);
            }
            if (e.damage_percentage_boost != 0) {
                StatEntry s; s.label = "Damage"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.damage_percentage_boost * 100);
                stats.push_back(s);
            }
            if (e.speed_boost != 0) {
                StatEntry s; s.label = "Speed"; 
                snprintf(s.value, sizeof(s.value), "%+.1f", e.speed_boost);
                stats.push_back(s);
            }
            if (e.speed_percentage_boost != 0) {
                StatEntry s; s.label = "Speed"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.speed_percentage_boost * 100);
                stats.push_back(s);
            }
            if (e.range_boost != 0) {
                StatEntry s; s.label = "Range"; 
                snprintf(s.value, sizeof(s.value), "%+.1f", e.range_boost);
                stats.push_back(s);
            }
            if (e.range_percentage_boost != 0) {
                StatEntry s; s.label = "Range"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.range_percentage_boost * 100);
                stats.push_back(s);
            }
            if (e.atk_cooldown_reduction != 0) {
                StatEntry s; s.label = "Attack Speed"; 
                snprintf(s.value, sizeof(s.value), "%+ds", e.atk_cooldown_reduction);
                stats.push_back(s);
            }
            if (e.atk_cooldown_percent_reduction != 0) {
                StatEntry s; s.label = "Attack Speed"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.atk_cooldown_percent_reduction * 100);
                stats.push_back(s);
            }
            if (e.healing != 0) {
                StatEntry s; s.label = "Healing"; 
                snprintf(s.value, sizeof(s.value), "%+d", e.healing);
                stats.push_back(s);
            }
            if (e.healing_percentage != 0) {
                StatEntry s; s.label = "Healing"; 
                snprintf(s.value, sizeof(s.value), "%+.1f%%", e.healing_percentage * 100);
                stats.push_back(s);
            }
            
            // Display in two columns: left to right, then down
            ImGui::Columns(2, "ItemStatsColumns", false);
            ImGui::SetColumnWidth(0, 155);
            
            for (size_t i = 0; i < stats.size(); i++) {
                const auto& stat = stats[i];
                ImVec4 color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); // Green for positive
                if (stat.value[0] == '-') color = ImVec4(0.9f, 0.2f, 0.2f, 1.0f); // Red for negative
                ImGui::TextColored(color, "%s %s", stat.label, stat.value);
                
                // After displaying, move to next column if we just filled left column
                if (i % 2 == 0 && i < stats.size() - 1) {
                    ImGui::NextColumn();
                }
            }
            
            ImGui::Columns(1);
            ImGui::SetWindowFontScale(1.0f); // Reset font scale
            ImGui::PopStyleVar();
            
            ImGui::EndGroup();
            
            // Discard button at bottom right - use fixed offset from right edge
            float button_width = 85.0f;
            float button_padding = 10.0f; // Padding from right edge
            float child_window_width = ImGui::GetWindowSize().x;
            float button_x = child_window_width - button_width - button_padding;
            float button_y = item_height - 28;
            ImGui::SetCursorPos(ImVec2(button_x, button_y));
            
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.3f, 0.3f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.9f, 0.1f, 0.1f, 1.0f));
            
            if (ImGui::Button(("Discard##" + std::to_string(item_id)).c_str(), ImVec2(85, 20))) {
                request_item_discard(item_id);
            }
            
            ImGui::PopStyleColor(3);
            
            ImGui::EndChild();
            ImGui::PopStyleColor();
            ImGui::PopStyleVar(2);
            
            ImGui::Spacing();
        }
    }
    
    ImGui::EndChild(); // End ItemsScrollRegion
    ImGui::EndChild(); // End RightPanel
    
    ImGui::End();
    ImGui::PopStyleVar(2);
}
