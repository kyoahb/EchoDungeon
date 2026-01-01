#pragma once
#include "Imports/common.h"
#include "Game/World/Assets/AssetMap.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Networking/Packet/Instances/PlayerInput.h"
#include "Networking/Packet/Instances/WorldSnapshot.h"
#include "Networking/Packet/Instances/EntityUpdate.h"
#include <unordered_map>
#include <string>
#include <vector>

class Client;  // Forward declaration

/**
 * @brief Client-side world state manager.
 * Renders world state and sends local player input to server.
 * Receives updates from server and applies them to local state.
 */
class ClientWorldManager {
public:
    ClientWorldManager(std::shared_ptr<Client> client);
    ~ClientWorldManager() = default;

    void update(float delta_time);  // Called every frame
    void draw_3d();  // Draw 3D entities
    void draw_2d();  // Draw 2D UI elements
    void clear();  // Clear all entities

    Player* get_local_player();
    
    void add_player(const Player& player);
    void remove_player(uint16_t peer_id);
    void update_player(uint16_t peer_id, const ObjectTransform& transform, float health);
    Player* get_player(uint16_t peer_id);
    const std::unordered_map<uint16_t, Player>& get_all_players() const { return players; }
    
    void add_object(const Object& object);
    void remove_object(uint16_t object_id);
    void update_object(uint16_t object_id, const ObjectTransform& transform);
    Object* get_object(uint16_t object_id);

    void apply_world_snapshot(const WorldSnapshotPacket& snapshot);
    void apply_entity_updates(const std::vector<EntityUpdateData>& updates);
    void send_local_player_input();  // Send local player transform to server

    raylib::Camera3D& get_camera() { return camera; }
    void update_camera(float delta_time);  // Update camera to follow local player

private:
    std::shared_ptr<Client> client;  // Reference to client for sending packets
    
    // World state
    std::unordered_map<uint16_t, Player> players;  // Keyed by peer_id
    std::unordered_map<uint16_t, Object> objects;  // Keyed by object_id
    
    // Camera
    raylib::Camera3D camera;
    
    // Input tracking
    std::chrono::steady_clock::time_point last_input_send_time;
    const float input_send_interval = 1.0f / 60.0f;  // Send input 60 times per second
    ObjectTransform last_sent_transform;  // Track last sent transform to avoid redundant sends
    
    // Helper methods
    void process_local_player_input(float delta_time);
    bool has_local_player_moved() const;
};
