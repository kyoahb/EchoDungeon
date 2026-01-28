#include "ServerWorldManager.h"
#include "Networking/Server/Server.h"
#include <sstream>
#include <iomanip>

ServerWorldManager::ServerWorldManager(std::shared_ptr<Server> server)
    : server(server), last_update_time(std::chrono::steady_clock::now()) {
}

void ServerWorldManager::update(float delta_time) {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_update_time);
    
    // Send entity updates at fixed interval (20 Hz)
    if (elapsed.count() >= update_interval) {
        broadcast_entity_updates();
        last_update_time = now;
    }
    
    // Update collisions
	PhysicsManager::update(&players, &enemies, &objects, this, nullptr);
}

void ServerWorldManager::clear() {
    players.clear();
    objects.clear();
    next_object_id = 0;
}


void ServerWorldManager::add_player(uint32_t peer_id, const std::string& name) {
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
        player.asset_id
    );
    server->broadcast_packet(packet);
}

void ServerWorldManager::remove_player(uint32_t peer_id) {
if (players.erase(peer_id) > 0) {
        // Broadcast player destroy
        PlayerDestroyPacket packet(peer_id);
        server->broadcast_packet(packet);
    }
}

Player* ServerWorldManager::get_player(uint32_t peer_id) {
    auto it = players.find(peer_id);
    return (it != players.end()) ? &it->second : nullptr;
}


uint32_t ServerWorldManager::spawn_object(ObjectType type, const std::string& asset_id, const raylib::Vector3& position) {
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
if (objects.erase(object_id) > 0) {
        // Broadcast object destroy
        ObjectDestroyPacket packet(object_id);
        server->broadcast_packet(packet);
    }
}

Object* ServerWorldManager::get_object(uint32_t object_id) {
    auto it = objects.find(object_id);
    return (it != objects.end()) ? &it->second : nullptr;
}

uint32_t ServerWorldManager::spawn_enemy(float max_health, float speed, float damage, 
    const std::string& asset_id, const raylib::Vector3& position) {

    static uint32_t next_enemy_id = 1;
    uint32_t enemy_id = next_enemy_id++;
    
    Enemy enemy(enemy_id, max_health, speed, damage, asset_id);
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
        enemy.asset_id
    );
    server->broadcast_packet(packet);
    
    return enemy_id;
}

Enemy* ServerWorldManager::get_enemy(uint32_t enemy_id) {
    auto it = enemies.find(enemy_id);
    return (it != enemies.end()) ? &it->second : nullptr;
}

const std::unordered_map<uint32_t, Enemy>& ServerWorldManager::get_all_enemies() const {
    return enemies; 
}

void ServerWorldManager::destroy_enemy(uint32_t enemy_id) {
    if (enemies.erase(enemy_id) > 0) {
        // Broadcast enemy destroy
        EnemyDestroyPacket packet(enemy_id);
        server->broadcast_packet(packet);
    }
}

void ServerWorldManager::broadcast_world_snapshot() {
    WorldSnapshotPacket packet(players, objects);
    server->broadcast_packet(packet);
}

void ServerWorldManager::send_world_snapshot(ENetPeer* peer) {
    WorldSnapshotPacket packet(players, objects);
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
Player* player = get_player(peer_id);
    if (!player) return;
    
    // Validate the transform (anti-cheat, collision, etc.)
    if (validate_player_transform(*player, input_transform)) {
        player->transform = input_transform;
        // Updates will be broadcast in the next update() call
    }
}

bool ServerWorldManager::validate_player_transform(const Player& player, const ObjectTransform& new_transform) {
    // TODO: Add validation logic
    // - Check speed
    // - Check collision with world geometry
    // - Check bounds (prevent going out of map)
    
    // For now, accept all transforms
    return true;
}
