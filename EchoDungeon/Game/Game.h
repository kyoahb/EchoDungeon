#pragma once
#include "Imports/common.h"
#include "Game/State/GameStateManager.h"
#include "Networking/Client/Client.h"
#include "Networking/Server/Server.h"
#include "GameSettings.h"

class Game {
public:

	raylib::Window window; // Storing an instance of the window 
	GameStateManager state_manager; // Manages the different game states, updated in update()
	std::shared_ptr<Client> client; // The client instance for networking
	std::shared_ptr<Server> server; // The server instance for networking
	GameSettings settings; // Game settings

	Game(); // Initialiser
	~Game(); // De-initialiser

	void begin(); // Start the game and enter the main update loop
	void update(); // Update the game state

};


