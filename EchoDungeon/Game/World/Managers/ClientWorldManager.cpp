#include "ClientWorldManager.h"
#include "Networking/Client/Client.h"
#include "Utils/Input.h"

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
    // Process local player input
    if (client->peers.local_server_side_id != 0) {
        process_local_player_input(delta_time);
        
        // Send input to server if enough time has passed
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_input_send_time);
        
        if (elapsed.count() >= input_send_interval && has_local_player_moved()) {
            send_local_player_input();
            last_input_send_time = now;
        }
    }
    
    // Update camera to follow local player
    update_camera(delta_time);
    
    // Apply physics (collision checking)
	PhysicsManager::update(&players, &objects);
}

void ClientWorldManager::draw_3d() {
    // Draw all players
    for (auto& [peer_id, player] : players) {
        player.draw3D(camera);
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
}

void ClientWorldManager::clear() {
    players.clear();
    objects.clear();
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

void ClientWorldManager::remove_player(uint16_t peer_id) {
    players.erase(peer_id);
}

void ClientWorldManager::update_player(uint16_t peer_id, const ObjectTransform& transform, float health) {
    auto it = players.find(peer_id);
    if (it != players.end()) {
        // Don't update local player from server (client-side prediction)
        if (peer_id != client->peers.local_server_side_id) {
            it->second.transform = transform;
            it->second.health = health;
        }
    }
}

Player* ClientWorldManager::get_player(uint16_t peer_id) {
    auto it = players.find(peer_id);
    return (it != players.end()) ? &it->second : nullptr;
}



void ClientWorldManager::add_object(const Object& object) {
    objects[object.id] = object;
}

void ClientWorldManager::remove_object(uint16_t object_id) {
    objects.erase(object_id);
}

void ClientWorldManager::update_object(uint16_t object_id, const ObjectTransform& transform) {
    auto it = objects.find(object_id);
    if (it != objects.end()) {
        it->second.transform = transform;
    }
}

Object* ClientWorldManager::get_object(uint16_t object_id) {
    auto it = objects.find(object_id);
    return (it != objects.end()) ? &it->second : nullptr;
}



void ClientWorldManager::apply_world_snapshot(const WorldSnapshotPacket& snapshot) {
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
}

void ClientWorldManager::apply_entity_updates(const std::vector<EntityUpdateData>& updates) {
    for (const auto& update : updates) {

		if (update.entity_type == EntityType::PLAYER) {
            update_player(update.entity_id, update.transform, update.health);
        }
        else if (update.entity_type == EntityType::NPC) {
            // For later npcs eg enemies
        }
        else if (update.entity_type == EntityType::OBJECT) {
            update_object(update.entity_id, update.transform);
		}
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
    
    // Get input
    raylib::Vector3 movement = {0.0f, 0.0f, 0.0f};
    
    if (Input::is_key_down(KEY_W)) movement.z -= 1.0f;
    if (Input::is_key_down(KEY_S)) movement.z += 1.0f;
    if (Input::is_key_down(KEY_A)) movement.x -= 1.0f;
    if (Input::is_key_down(KEY_D)) movement.x += 1.0f;
    
    // Normalize and apply speed
    if (movement.x != 0.0f || movement.z != 0.0f) {
        float speed = 5.0f;
        
        // Apply movement
        local_player->move(movement.Normalize() * speed * delta_time);
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
