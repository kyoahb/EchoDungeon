#pragma once
#include "Game/State/GameState.h"
#include <atomic>
#include "Game/World/Entities/Player.h"

class World : public GameState {
public:
	World(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

	std::unique_ptr<Player> local_player; // The local player instance
	raylib::Camera3D camera; // The 3D camera for the world
};