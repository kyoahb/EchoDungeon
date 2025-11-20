#pragma once
#include "Game/State/GameState.h"

class Settings : public GameState {
public:
	Settings(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

};