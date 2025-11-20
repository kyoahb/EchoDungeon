#pragma once
#include "Imports/common.h"
#include "Game/State/GameStateManager.h"

class Game {
public:

	Game(); // Initialiser
	~Game(); // De-initialiser

	void begin(); // Start the game and enter the main update loop

	raylib::Window window; // Storing an instance of the window 
	GameStateManager state_manager; // Manages the different game states, updated in update()

	void update(); // Update the game state

};


