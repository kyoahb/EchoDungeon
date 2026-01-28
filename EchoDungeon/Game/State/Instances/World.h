#pragma once
#include "Game/State/GameState.h"
#include <atomic>
#include "Game/World/Entities/Player.h"
#include "Game/World/Managers/ClientWorldManager.h"
#include "Game/World/Managers/ServerWorldManager.h"

class World : public GameState {
public:
	World(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

	std::unique_ptr<ClientWorldManager> c_world_manager; // Client-side world manager
	std::unique_ptr<ServerWorldManager> s_world_manager; // Server-side world manager (only if hosting)

private:
	// Setup methods
	void setup_client_events();
	void setup_server_events();

	// Client-side event subscription IDs
	int client_world_snapshot_sub = -1;
	int client_player_spawn_sub = -1;
	int client_player_update_sub = -1;
	int client_player_destroy_sub = -1;
	int client_enemy_spawn_sub = -1;
	int client_enemy_update_sub = -1;
	int client_enemy_destroy_sub = -1;
	int client_object_spawn_sub = -1;
	int client_object_destroy_sub = -1;

	// Server-side event subscription IDs
	int server_player_input_sub = -1;
	int server_request_snapshot_sub = -1;
	int server_player_disconnect_sub = -1;
	int server_player_disconnect_timeout_sub = -1;

	std::atomic<bool> should_quit_to_mainmenu = false;
};