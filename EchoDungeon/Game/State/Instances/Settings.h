#pragma once
#include "Game/State/GameState.h"
#include "Game/GameSettings.h"

class Settings : public GameState {
public:
	Settings(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

	GameSettings settings_buffer; // Buffer for settings changes
};