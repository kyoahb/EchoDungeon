#pragma once
#include "Imports/common.h"
#include "Game/World/Assets/AssetMap.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Game/World/Entities/Item.h"
#include "Networking/Packet/Instances/WorldSnapshot.h"
#include "Networking/Packet/Instances/Player/PlayerUpdate.h"
#include "Networking/Packet/Instances/Enemy/EnemyUpdate.h"
#include <unordered_map>
#include <string>
#include <vector>
#include "PhysicsManager.h"
#include "Game/World/Entities/Enemy.h"

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
    void remove_player(uint32_t peer_id);
    void update_player(uint32_t peer_id, const ObjectTransform& transform, float health,
        float damage, float max_health, float range, float speed, uint64_t attack_cooldown, 
        uint64_t last_attack_time, bool attacking, const Inventory& inventory);
    Player* get_player(uint32_t peer_id);
    const std::unordered_map<uint32_t, Player>& get_all_players() const { return players; }
    
    void add_object(const Object& object);
    void remove_object(uint32_t object_id);
    Object* get_object(uint32_t object_id);
    
    void add_enemy(const Enemy& enemy);
    void remove_enemy(uint32_t enemy_id);
    void update_enemy(uint32_t enemy_id, const ObjectTransform& transform, float health);
    Enemy* get_enemy(uint32_t enemy_id);

    void apply_world_snapshot(const WorldSnapshotPacket& snapshot);
    void apply_player_updates(const std::vector<PlayerUpdateData>& updates);
    void apply_enemy_updates(const std::vector<EnemyUpdateData>& updates);
    void send_local_player_input();  // Send local player transform to server

    // Item system
    void handle_item_pickup(uint32_t player_id, const Item& item);
    void request_item_discard(uint32_t item_id);
    void toggle_inventory();
    void draw_inventory_ui();
    Item* get_item(uint32_t item_id);

    raylib::Camera3D& get_camera() { return camera; }
    void update_camera(float delta_time);  // Update camera to follow local player

private:
    std::shared_ptr<Client> client;  // Reference to client for sending packets
    
    // World state
    std::unordered_map<uint32_t, Player> players;  // Keyed by peer_id
    std::unordered_map<uint32_t, Object> objects;  // Keyed by object_id
    std::unordered_map<uint32_t, Enemy> enemies;  // Keyed by enemy_id
    std::unordered_map<uint32_t, Item> items;  // Client-side item copies
    
    // Thread synchronization for world state
    mutable std::mutex world_state_mutex;
    
    // Camera
    raylib::Camera3D camera;
    
    // Inventory UI
    bool show_inventory = false;
    
    // Input tracking
    std::chrono::steady_clock::time_point last_input_send_time;
    const float input_send_interval = 1.0f / 60.0f;  // Send input 60 times per second
    ObjectTransform last_sent_transform;  // Track last sent transform to avoid redundant sends
    
    // Helper methods
    void process_local_player_input(float delta_time);
    bool has_local_player_moved() const;
};
