#pragma once
#include "Imports/common.h"

class Game {
public:

	Game(); // Initialiser
	~Game(); // De-initialiser

	void begin(); // Start the game and enter the main update loop

private:
	raylib::Window window; // Storing an instance of the window 

	void update(); // Update the game state

};


