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
	PhysicsManager::update(&players, &objects);
}

void ServerWorldManager::clear() {
    players.clear();
    objects.clear();
    next_object_id = 0;
}


void ServerWorldManager::add_player(uint16_t peer_id, const std::string& name) {
    Player player(peer_id, false, name);
    player.transform.set_position({0.0f, 1.0f, 0.0f});  // Spawn at origin
    players[peer_id] = player;
    
    // Broadcast player spawn to all clients
    PlayerSpawnPacket packet(player);
    server->broadcast_packet(packet);
}

void ServerWorldManager::remove_player(uint16_t peer_id) {
    if (players.erase(peer_id) > 0) {
        // Broadcast entity destroy
        EntityDestroyPacket packet(peer_id, EntityType::PLAYER);
        server->broadcast_packet(packet);
    }
}

Player* ServerWorldManager::get_player(uint16_t peer_id) {
    auto it = players.find(peer_id);
    return (it != players.end()) ? &it->second : nullptr;
}


uint16_t ServerWorldManager::spawn_object(ObjectType type, const std::string& asset_id, const raylib::Vector3& position) {
    uint16_t object_id = next_object_id++;
    
    Object obj(object_id, asset_id, type);
    obj.transform.set_position(position);
    objects[object_id] = obj;
    
    // Broadcast object spawn to all clients
    EntitySpawnPacket packet(obj);
    server->broadcast_packet(packet);
    
    return object_id;
}

void ServerWorldManager::destroy_object(uint16_t object_id) {
    if (objects.erase(object_id) > 0) {
        // Broadcast entity destroy
        EntityDestroyPacket packet(object_id, EntityType::OBJECT);
        server->broadcast_packet(packet);
    }
}

Object* ServerWorldManager::get_object(uint16_t object_id) {
    auto it = objects.find(object_id);
    return (it != objects.end()) ? &it->second : nullptr;
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
    std::vector<EntityUpdateData> updates;
    collect_entity_updates(updates);
    
    if (!updates.empty()) {
        EntityUpdatePacket packet(updates);
        server->broadcast_packet(packet);
    }
}

void ServerWorldManager::collect_entity_updates(std::vector<EntityUpdateData>& updates) {
    // Collect player updates
    for (const auto& [peer_id, player] : players) {
        EntityUpdateData update;
        update.entity_id = peer_id;
        update.entity_type = EntityType::PLAYER;
        update.transform = player.transform;
        update.health = player.health;
        updates.push_back(update);
    }
    
    // Collect dynamic object updates (skip static objects)
    for (const auto& [object_id, object] : objects) {
        if (!object.transform.get_is_static()) {
            EntityUpdateData update;
            update.entity_id = object_id;
            update.entity_type = EntityType::OBJECT;
            update.transform = object.transform;
            update.health = 0.0f;  // Objects don't have health
            updates.push_back(update);
        }
    }
}



void ServerWorldManager::handle_player_input(uint16_t peer_id, const ObjectTransform& input_transform) {
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
