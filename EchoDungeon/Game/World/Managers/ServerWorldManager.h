#pragma once
#include "Imports/common.h"
#include "Game/World/Assets/AssetMap.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Game/World/Entities/Item.h"
#include "Networking/Packet/Instances/WorldSnapshot.h"
#include "Networking/Packet/Instances/Player/PlayerSpawn.h"
#include "Networking/Packet/Instances/Player/PlayerUpdate.h"
#include "Networking/Packet/Instances/Player/PlayerDestroy.h"
#include "Networking/Packet/Instances/Enemy/EnemySpawn.h"
#include "Networking/Packet/Instances/Enemy/EnemyUpdate.h"
#include "Networking/Packet/Instances/Enemy/EnemyDestroy.h"
#include "Networking/Packet/Instances/Object/ObjectSpawn.h"
#include "Networking/Packet/Instances/Object/ObjectDestroy.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "PhysicsManager.h"
#include "Game/World/Entities/Enemy.h"

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

    void add_player(uint32_t peer_id, const std::string& name);
    void remove_player(uint32_t peer_id);
    Player* get_player(uint32_t peer_id);
    const std::unordered_map<uint32_t, Player>& get_all_players() const { return players; }
    
    uint32_t spawn_object(ObjectType type, const std::string& asset_id, const raylib::Vector3& position);
    void destroy_object(uint32_t object_id);
    Object* get_object(uint32_t object_id);
    const std::unordered_map<uint32_t, Object>& get_all_objects() const { return objects; }

	uint32_t spawn_enemy(float max_health, float speed, float damage,
        const std::string& asset_id, const raylib::Vector3& position);
    std::vector<uint32_t> spawn_enemies(float max_health, float speed, float damage,
        const std::string& asset_id, const raylib::Vector3 position, int count);
	Enemy* get_enemy(uint32_t enemy_id);
	const std::unordered_map<uint32_t, Enemy>& get_all_enemies() const { return enemies; }
	void destroy_enemy(uint32_t enemy_id);

    void broadcast_world_snapshot();  // Send full state to all clients
    void send_world_snapshot(ENetPeer* peer);  // Send full state to specific client
    void broadcast_entity_updates();  // Send delta updates (called every tick)

    void handle_player_input(uint32_t peer_id, const ObjectTransform& input_transform);
    void handle_player_attack(uint32_t peer_id);

	uint64_t get_elapsed_gametime() const;  // Num of ms passed since the game started

    // Item system
    uint32_t create_item_for_player(uint32_t player_id);
    void handle_item_discard(uint32_t player_id, uint32_t item_id);
    Item* get_item(uint32_t item_id);

private:
    std::shared_ptr<Server> server;  // Reference to server for broadcasting
    
    // World state
    std::unordered_map<uint32_t, Player> players;  // Keyed by peer_id
    std::unordered_map<uint32_t, Object> objects;  // Keyed by object id
	std::unordered_map<uint32_t, Enemy> enemies; // Keyed by enemy id
    std::unordered_map<uint32_t, Item> items;  // Keyed by item id

    
    // Entity ID generation
    uint32_t next_object_id = 1;
    uint32_t next_item_id = 1;
    
    // Item drop configuration
    float item_drop_chance = 0.3f;  // 30% chance to drop item on enemy death
    
    // Game start time for difficulty scaling
    std::chrono::steady_clock::time_point game_start_time;
    
    // Update tracking
    std::chrono::steady_clock::time_point last_update_time;
    const float update_interval = 1.0f / 30.0f;  // 30 updates per second
    
    // Helper methods
    void collect_player_updates(std::vector<PlayerUpdateData>& updates);
    void collect_enemy_updates(std::vector<EnemyUpdateData>& updates);
    bool validate_player_transform(const Player& player, const ObjectTransform& new_transform);
};