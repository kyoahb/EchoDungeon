#include "ServerWorldManager.h"
#include "Networking/Server/Server.h"
#include "Utils/NetUtils.h"
#include "Game/World/Systems/ItemGenerator.h"
#include "Networking/Packet/Instances/Item/ItemPickup.h"
#include <sstream>
#include <iomanip>

ServerWorldManager::ServerWorldManager(std::shared_ptr<Server> server)
    : server(server), last_update_time(std::chrono::steady_clock::now()), 
      game_start_time(std::chrono::steady_clock::now()) {
}

void ServerWorldManager::update(float delta_time) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update_time);
    
    // Spawn new enemies over time
	regular_enemy_spawning_update(delta_time);

    // Update enemies
    for (auto& [enemy_id, enemy] : enemies) {
        // Gather pointers to all players for enemy AI
        std::vector<Player*> player_ptrs;
        for (auto& [peer_id, player] : players) {
            player_ptrs.push_back(&player);     
        }
        enemy.tick(delta_time, player_ptrs);
    }

    // Update player .attacking
	uint64_t current_time = NetUtils::get_current_time_millis();
    for (auto& [peer_id, player] : players) {
        if (player.attacking && 
            (current_time - player.last_attack_time) >= player.attack_cooldown) {
            player.attacking = false; // Reset attacking state after cooldown
        }
	}


    // Update collisions
    PhysicsManager::update(&players, &enemies, &objects, this, nullptr);

    // Send entity updates at fixed interval (20 Hz)
    if (elapsed.count() >= update_interval) {
        broadcast_entity_updates();
        last_update_time = now;
    }
}

void ServerWorldManager::clear() {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    players.clear();
    objects.clear();
    items.clear();
    next_object_id = 0;
    next_item_id = 1;
}


void ServerWorldManager::add_player(uint32_t peer_id, const std::string& name) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    Player player(peer_id, false, name);
    player.transform.set_position({0.0f, 1.0f, 0.0f});  // Spawn at origin
    players[peer_id] = player;
    
    // Broadcast player spawn to all clients with individual fields
    PlayerSpawnPacket packet(
        player.id,
        player.name,
        player.transform,
        player.health,
        player.max_health,
        player.damage,
        player.range,
        player.speed,
		player.attack_cooldown,
        player.last_attack_time,
        player.asset_id,
        player.inventory
    );
    server->broadcast_packet(packet);
}

void ServerWorldManager::remove_player(uint32_t peer_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    if (players.erase(peer_id) > 0) {
        // Broadcast player destroy
        PlayerDestroyPacket packet(peer_id);
        server->broadcast_packet(packet);
    }
}

Player* ServerWorldManager::get_player(uint32_t peer_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = players.find(peer_id);
    return (it != players.end()) ? &it->second : nullptr;
}


uint32_t ServerWorldManager::spawn_object(ObjectType type, const std::string& asset_id, const raylib::Vector3& position) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    uint32_t object_id = next_object_id++;
    
    Object obj(object_id, asset_id, type);
    obj.transform.set_position(position);
    objects[object_id] = obj;
    
    // Broadcast object spawn to all clients with individual fields
    ObjectSpawnPacket packet(
        object_id,
        asset_id,
        type,
        obj.transform
    );
    server->broadcast_packet(packet);
    
    return object_id;
}

void ServerWorldManager::destroy_object(uint32_t object_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    if (objects.erase(object_id) > 0) {
        // Broadcast object destroy
        ObjectDestroyPacket packet(object_id);
        server->broadcast_packet(packet);
    }
}

Object* ServerWorldManager::get_object(uint32_t object_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = objects.find(object_id);
    return (it != objects.end()) ? &it->second : nullptr;
}

uint32_t ServerWorldManager::spawn_enemy(float max_health, float speed, float damage,
    const raylib::Vector3& position) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);

    static uint32_t next_enemy_id = 1;
    uint32_t enemy_id = next_enemy_id++;

    std::string asset_id = "zombie";
    bool drops_items = false;

    // Get RNG drop
    float roll = (float)rand() / RAND_MAX;
    if (roll <= item_drop_chance) {
        asset_id = "goldzombie";
        drops_items = true;
    }
    
    Enemy enemy(enemy_id, max_health, speed, damage, asset_id);
	enemy.spawns_items = drops_items;
    enemy.transform.set_position(position);
    enemies[enemy_id] = enemy;
    
    // Broadcast enemy spawn to all clients with individual fields
    EnemySpawnPacket packet(
        enemy.id,
        enemy.transform,
        enemy.health,
        enemy.max_health,
        enemy.damage,
        enemy.speed,
        enemy.spawns_items,
        enemy.asset_id
    );
    server->broadcast_packet(packet);

	INFO("SERVER-SIDE: Spawned enemy ID " + std::to_string(enemy_id) +
         " at position (" + std::to_string(position.x) + ", " + 
         std::to_string(position.y) + ", " + std::to_string(position.z) + ") is gold: " + std::to_string(drops_items));
    
    return enemy_id;
}

Enemy* ServerWorldManager::get_enemy(uint32_t enemy_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = enemies.find(enemy_id);
    return (it != enemies.end()) ? &it->second : nullptr;
}

void ServerWorldManager::destroy_enemy(uint32_t enemy_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    if (enemies.erase(enemy_id) > 0) {
        // Broadcast enemy destroy
        INFO("Destroyed enemy: " + std::to_string(enemy_id));
        EnemyDestroyPacket packet(enemy_id);
        server->broadcast_packet(packet);
    }
}

void ServerWorldManager::broadcast_world_snapshot() {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    WorldSnapshotPacket packet(players, objects, enemies);
    server->broadcast_packet(packet);
}

void ServerWorldManager::send_world_snapshot(ENetPeer* peer) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    WorldSnapshotPacket packet(players, objects, enemies);
    enet_peer_send(peer, 0, packet.to_enet_packet());
}

void ServerWorldManager::broadcast_entity_updates() {
    // Collect and broadcast player updates
    std::vector<PlayerUpdateData> player_updates;
    collect_player_updates(player_updates);
    if (!player_updates.empty()) {
        PlayerUpdatePacket player_packet(player_updates);
        server->broadcast_packet(player_packet);
    }
    
    // Collect and broadcast enemy updates
    std::vector<EnemyUpdateData> enemy_updates;
    collect_enemy_updates(enemy_updates);
    if (!enemy_updates.empty()) {
        EnemyUpdatePacket enemy_packet(enemy_updates);
        server->broadcast_packet(enemy_packet);
    }
}

void ServerWorldManager::collect_player_updates(std::vector<PlayerUpdateData>& updates) {
    // Collect player updates
    for (const auto& [peer_id, player] : players) {
        PlayerUpdateData update;
        update.id = peer_id;
        update.transform = player.transform;
        update.health = player.health;
		update.damage = player.damage;
		update.max_health = player.max_health;
		update.range = player.range;
		update.speed = player.speed;
		update.attack_cooldown = player.attack_cooldown;
		update.last_attack_time = player.last_attack_time;
		update.attacking = player.attacking;
		update.inventory = player.inventory;
        updates.push_back(update);
    }
}

void ServerWorldManager::collect_enemy_updates(std::vector<EnemyUpdateData>& updates) {
    // Collect enemy updates
    for (const auto& [enemy_id, enemy] : enemies) {
        EnemyUpdateData update;
        update.id = enemy_id;
        update.transform = enemy.transform;
        update.health = enemy.health;
        updates.push_back(update);
    }
}



void ServerWorldManager::handle_player_input(uint32_t peer_id, const ObjectTransform& input_transform) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = players.find(peer_id);
    if (it == players.end()) return;
    Player* player = &it->second;
    
    // Ensure player is not dead
	if (player->is_dead()) return;

    // Validate the transform (anti-cheat, collision, etc.)
    if (validate_player_transform(*player, input_transform)) {
        player->transform = input_transform;
        // Updates will be broadcast in the next update() call
    }
}

void ServerWorldManager::handle_player_attack(uint32_t peer_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto player_it = players.find(peer_id);
    if (player_it == players.end()) return;
    Player* player = &player_it->second;

    // Ensure player is not dead
    if (player->is_dead()) return;

    uint64_t current_time = NetUtils::get_current_time_millis();

    // Check cooldown
    if ((current_time - player->last_attack_time) < player->attack_cooldown) {
        return; // Still on cooldown, ignore attack
    }

    // Reset attack cooldown with current timestamp
    player->attacking = true;
    player->last_attack_time = current_time;

    // Get player position
    raylib::Vector3 player_pos = player->transform.get_position();

    // Find and damage all enemies within range
    std::vector<uint32_t> enemies_to_destroy;
    for (auto& [enemy_id, enemy] : enemies) {
        raylib::Vector3 enemy_pos = enemy.transform.get_position();
        
        // Circle collision check (distance <= range)
            float distance = player_pos.Distance(enemy_pos);
            if (distance <= player->range) {
                // Apply damage
                enemy.health -= player->damage;
            
                INFO("Player " + std::to_string(peer_id) + " attacked enemy " + std::to_string(enemy_id) + 
                     " for " + std::to_string(player->damage) + " damage");
            
                // Mark for destruction if dead
                if (enemy.health <= 0.0f) {
                    enemies_to_destroy.push_back(enemy_id);

					if (enemy.spawns_items) {
                        create_item_for_player_internal(peer_id);
                    }
                }
            }
        }

    // Destroy dead enemies (without lock since we already hold it)
    for (uint32_t enemy_id : enemies_to_destroy) {
        if (enemies.erase(enemy_id) > 0) {
            INFO("Destroyed enemy: " + std::to_string(enemy_id));
            EnemyDestroyPacket packet(enemy_id);
            server->broadcast_packet(packet);
        }
    }
}

bool ServerWorldManager::validate_player_transform(const Player& player, const ObjectTransform& new_transform) {
    // For now, accept all transforms
    return true;
}

uint64_t ServerWorldManager::get_elapsed_gametime() const {
    auto now = std::chrono::steady_clock::now();
    uint64_t game_time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - game_start_time).count();
    
    return game_time_ms;
}

uint32_t ServerWorldManager::create_item_for_player(uint32_t player_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    return create_item_for_player_internal(player_id);
}

uint32_t ServerWorldManager::create_item_for_player_internal(uint32_t player_id) {
    // NOTE: Must be called with world_state_mutex already held
    auto it = players.find(player_id);
    if (it == players.end()) return 0;
    Player* player = &it->second;
    
    // Generate random item (use ItemGenerator)
    uint32_t item_id = next_item_id++;
    Item item = ItemGenerator::generate_random_item(item_id, get_elapsed_gametime());
    
    // Store item in registry
    items[item_id] = item;
    
    // Add to player inventory
    player->inventory.add_item(item_id);
    
    // Apply item effects to player (including healing)
    player->apply_item_effects(item.effects);
    
    // Broadcast item pickup to all clients
    ItemPickupPacket packet(player_id, item);
    server->broadcast_packet(packet);
    
    INFO("Player " + std::to_string(player_id) + 
         " received item: " + item.item_name);
    
    return item_id;
}

void ServerWorldManager::handle_item_discard(uint32_t player_id, uint32_t item_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = players.find(player_id);
    if (it == players.end()) return;
    Player* player = &it->second;
    
    // Check if player has this item
    if (!player->inventory.has_item(item_id)) {
        ERROR("Player " + std::to_string(player_id) + 
             " tried to discard item they don't have: " + 
             std::to_string(item_id));
        return;
    }
    
    // Remove from inventory and recalculate stats
    player->remove_item_effects(item_id, items);
    
    // Remove from item registry
    items.erase(item_id);
    
    INFO("Player " + std::to_string(player_id) + 
         " discarded item: " + std::to_string(item_id));
}

Item* ServerWorldManager::get_item(uint32_t item_id) {
    std::lock_guard<std::recursive_mutex> lock(world_state_mutex);
    
    auto it = items.find(item_id);
    return (it != items.end()) ? &it->second : nullptr;
}

void ServerWorldManager::regular_enemy_spawning_update(float delta_time) {
    // Get server time
    uint64_t time = get_elapsed_gametime();
    float seconds = time / 1000.0f;

	// Calculate spawn interval based on time
    float spawn_interval = 10.0f * (std::exp(-seconds/200.0f));
    // At 0s: 10s interval
	// At 60s: ~7.4s interval
	// At 600s: ~0.5s interval
    // At 2400s: ~0.00006s interval
    
    if (NetUtils::get_current_time_millis() - last_enemy_spawn_time >= static_cast<uint64_t>(spawn_interval*1000)) {
        last_enemy_spawn_time = NetUtils::get_current_time_millis();
        
        // Spawn enemy around random player
		if (players.empty()) return; // No players to spawn near

        // Select a random player
		auto it = players.begin();
		std::advance(it, rand() % players.size());
		Player& target_player = it->second;
		raylib::Vector3 player_pos = target_player.transform.get_position();

		// Random offset within 10 to 20 units
		float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * PI;
		float distance = 5.0f + static_cast<float>(rand()) / RAND_MAX * 10.0f; // 5 to 15 units
		float x = player_pos.x + distance * cos(angle);
		float z = player_pos.z + distance * sin(angle);
        
        // Scale enemy stats based on elapsed game time
        uint64_t elapsed_time = get_elapsed_gametime();
        float health = 50.0f + (elapsed_time / 60000.0f) * 10.0f; // +5 health per minute
        float speed = 1.0f + (elapsed_time / 60000.0f) * 0.1f;    // +0.05 speed per minute
        float damage = 5.0f + (elapsed_time / 60000.0f) * 1.0f;   // +1 damage per minute

        spawn_enemy(health, speed, damage, raylib::Vector3{ x, 1.0f, z });
    }
}