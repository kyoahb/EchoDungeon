#include "World.h"
#include "Game/Game.h"
#include "Utils/Input.h"

World::World(Game& game) : GameState(game) {
	// Player is created in on_activate when client is available
	camera.position = { 0.0f, 10.0f, 0.0f };  // Camera above, looking down
	camera.target = { 0.0f, 0.0f, 0.0f };      // Looking at origin
	camera.up = { 0.0f, 0.0f, -1.0f };         // "Up" is -Z for top-down (so +Y on screen is -Z in world)
	camera.fovy = 70.0f;                        // Field of view
	camera.projection = CAMERA_PERSPECTIVE;    // Camera projection mode
}

void World::on_activate() {
	TRACE("World activated");
	local_player = std::make_unique<Player>(
		game.client ? game.client->peers.local_server_side_id : 0,
		true,
		game.settings.username);
}

void World::on_deactivate() {
	TRACE("World deactivated");
}

void World::update() {
	if (!local_player) {
		ERROR("No local player in World state update?");
		return;
	}

	// Handle player movement
	raylib::Vector3 movement = { 0.0f, 0.0f, 0.0f };
	const int move_speed = 2; // Units per second
	if (Input::is_key_down(KEY_W)) {
		movement.z -= 1; // Move forward (negative Z)
	}
	if (Input::is_key_down(KEY_S)) {
		movement.z += 1; // Move backward (positive Z)
	}
	if (Input::is_key_down(KEY_A)) {
		movement.x -= 1; // Move left (negative X)
	}
	if (Input::is_key_down(KEY_D)) {
		movement.x += 1; // Move right (positive X)
	}

	// Multiply by DeltaTime AND normalize to ensure consistent movement speed
	local_player->move(movement.Normalize() * move_speed * GetFrameTime());
	// Move the camera to follow the player. Only the player's X and Z positions change
	camera.position.x = local_player->position.x;
	camera.position.z = local_player->position.z;
	camera.target.x = local_player->position.x;
	camera.target.z = local_player->position.z;


	

	// Begin 3D rendering with top-down camera
	camera.BeginMode();

	// Draw a grid for reference
	DrawGrid(50, 1.0f);

	local_player->draw3D(); // Draw the player model in 3D


	camera.EndMode();

	// Draw 2D UI elements after 3D mode ends
	local_player->drawUI(camera); // Draw name labels in 2D

	// DEBUG: Draw hud info
	DrawFPS(10, 0);
	DrawText(("Player position:" + local_player->position.ToString()).c_str(), 10, 40, 20, BLACK);

	const std::vector<int>& keys = Input::get_keys_down();
	std::string keysText = "Pressed keys: ";
	for (int key : keys) {
		keysText += Input::get_key_name(key) + ", ";
	}
	DrawText(keysText.c_str(), 10, 80, 20, BLACK);
}