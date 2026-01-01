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
	std::unique_ptr<ServerWorldManager> s_world_manager; // Server-side world manager

	// Client-side event handlers
	int player_input_callback = -1; // Subscription ID for player input events
	int world_snapshot_request_callback = -1; // Subscription ID for world snapshot requests

	// Server-side event handlers
	int world_snapshot_callback = -1; // Subscription ID for world snapshot events
	int entity_update_callback = -1; // Subscription ID for entity update events
	int entity_destroy_callback = -1; // Subscription ID for entity destroy events
};