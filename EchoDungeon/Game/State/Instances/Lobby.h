#pragma once
#include "Game/State/GameState.h"
#include <atomic>

class Lobby : public GameState {
public:
	Lobby(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

	void handle_disconnect_event(const ClientEvents::DisconnectEventData& data); // Handle disconnect event
private:
	int on_disconnect_callback; // ID for disconnect callback
	std::atomic<bool> should_quit_to_mainmenu{false}; // Flag to handle disconnect on main thread
};