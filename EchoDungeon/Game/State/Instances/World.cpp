#include "World.h"
#include "Game/Game.h"
#include "Utils/Input.h"

World::World(Game& game) : GameState(game) {
	// Player is created in on_activate when client is available
}

void World::on_activate() {
	// Pass
	TRACE("World activated");

	// Create ClientWorldManager
	c_world_manager = std::make_unique<ClientWorldManager>(game.client);

	if (game.is_hosting()) {
		
		// Implement server world manager
		s_world_manager = std::make_unique<ServerWorldManager>(game.server);

		// Setup server events


		TRACE("ServerWorldManager created");
	}
}

void World::on_deactivate() {
	TRACE("World deactivated");
	// Clear and destroy ClientWorldManager
	if (c_world_manager) {
		c_world_manager->clear();
		c_world_manager.reset();
		c_world_manager = nullptr;
	}
}

void World::update() {
	if (!c_world_manager) return;

	if (c_world_manager->get_local_player() == nullptr) {
		// The client has not received a server loading packet yet
		DrawText("Loading world...", 10, 10, 20, BLACK);
		return;
	}
	// Update ClientWorldManager
	// This updates input, camera, sends input packets 
	c_world_manager->update(GetFrameTime());


	c_world_manager->get_camera().BeginMode();
	// Do all 3D Rendering

	c_world_manager->draw_3d();

	// End 3D Rendering
	c_world_manager->get_camera().EndMode();


	// Do all 2D Rendering
	c_world_manager->draw_2d();

	// DEBUG: Draw hud info
	DrawFPS(10, 0);

	const std::vector<int>& keys = Input::get_keys_down();
	std::string keysText = "Pressed keys: ";
	for (int key : keys) {
		keysText += Input::get_key_name(key) + ", ";
	}
	DrawText(keysText.c_str(), 10, 80, 20, BLACK);

	// End 2D Rendering
}