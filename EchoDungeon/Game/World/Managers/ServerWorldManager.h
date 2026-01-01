#pragma once
#include "Imports/common.h"
#include "Game/World/Assets/AssetMap.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Networking/Packet/Instances/WorldSnapshot.h"
#include "Networking/Packet/Instances/EntityUpdate.h"
#include "Networking/Packet/Instances/EntitySpawn.h"
#include "Networking/Packet/Instances/EntityDestroy.h"
#include "Networking/Packet/Instances/PlayerSpawn.h"
#include <unordered_map>
#include <string>
#include <memory>

class Server;  // Forward declaration

/**
 * @brief Server-side world state manager.
 * Authoritative source of truth for all game entities.
 * Validates client input and broadcasts updates.
 */
class ServerWorldManager {
public:
    ServerWorldManager(std::shared_ptr<Server> server);
    ~ServerWorldManager() = default;

    void update(float delta_time);  // Called every server tick
    void clear();  // Clear all entities (e.g., when changing levels)

    void add_player(uint16_t peer_id, const std::string& name);
    void remove_player(uint16_t peer_id);
    Player* get_player(uint16_t peer_id);
    const std::unordered_map<uint16_t, Player>& get_all_players() const { return players; }
    
    uint16_t spawn_object(ObjectType type, const std::string& asset_id, const raylib::Vector3& position);
    void destroy_object(uint16_t object_id);
    Object* get_object(uint16_t object_id);
    const std::unordered_map<uint16_t, Object>& get_all_objects() const { return objects; }

    void broadcast_world_snapshot();  // Send full state to all clients
    void send_world_snapshot(ENetPeer* peer);  // Send full state to specific client
    void broadcast_entity_updates();  // Send delta updates (called every tick)

    void handle_player_input(uint16_t peer_id, const ObjectTransform& input_transform);

private:
    std::shared_ptr<Server> server;  // Reference to server for broadcasting
    
    // World state
    std::unordered_map<uint16_t, Player> players;  // Keyed by peer_id
    std::unordered_map<uint16_t, Object> objects;  // Keyed by object id
    
    // Entity ID generation
    uint16_t next_object_id = 1;
    
    // Update tracking
    std::chrono::steady_clock::time_point last_update_time;
    const float update_interval = 1.0f / 20.0f;  // 20 updates per second
    
    // Helper methods
    void collect_entity_updates(std::vector<EntityUpdateData>& updates);
    bool validate_player_transform(const Player& player, const ObjectTransform& new_transform);
};