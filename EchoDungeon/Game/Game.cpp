#include "Game.h"
#include "Game/State/Instances/MainMenu.h"
#include "Game/State/Instances/Settings.h"
#include "Game/State/Instances/Host.h"
#include "Game/State/Instances/Join.h"
#include "Game/State/Instances/Lobby.h"
#include "Game/State/Instances/World.h"
#include "Networking/Packet/PacketRegistry.h"
#include "Utils/Input.h"

Game::Game() : state_manager(GameStateManager()){

	Logger::init(); // Initialise logger first
	if (enet_initialize() != 0) { // Initialise ENet
		ERROR("Failed to initialize ENet");
		throw std::runtime_error("Failed to initialize ENet");
	}
	atexit(enet_deinitialize); // Ensure ENet deinitializes on exit

	PacketRegistry::initializeRegistry(); // Initialize packet registry


	// Setup raylib window
	const int screenWidth = 1280;
	const int screenHeight = 720;

	window = raylib::Window(screenWidth, screenHeight, "Echo Dungeon");
	rlImGuiSetup(true); // Setup Raylib ImGUI connection

	// Load all models pre-emptively
	AssetMap::load();


	// Set default font to Arial, size 24
	ImGuiIO& io = ImGui::GetIO();
	ImFont* arialFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 24.0f);
	io.FontDefault = arialFont; // Set as default for larger text

	// Add all GameStates here
	state_manager.add_state("MainMenu", std::make_shared<MainMenu>(*this));
	state_manager.add_state("Settings", std::make_shared<Settings>(*this));
	state_manager.add_state("Host", std::make_shared<Host>(*this));
	state_manager.add_state("Join", std::make_shared<Join>(*this));
	state_manager.add_state("Lobby", std::make_shared<Lobby>(*this));
	state_manager.add_state("World", std::make_shared<World>(*this));

	state_manager.set_state("MainMenu"); // Set initial state

	TRACE("Game initialised");
}

Game::~Game() {
	rlImGuiShutdown(); // Shutdown Raylib ImGUI connection
	TRACE("Game de-initialised");
}

/**
 * @brief Start update loop while window is open.
 */
void Game::begin() {
	TRACE("Game started");
	while (!window.ShouldClose()) {
		update();
	}
}

/**
 * @brief Executed every frame to update the game state.
 */
void Game::update() {
	// Update input manager
	Input::update();

	// Draw
	BeginDrawing(); // Tell Raylib we are going to draw

	window.ClearBackground(GRAY); // Clear previous frame by replacing with full white
	rlImGuiBegin(); // Start Raylib-ImGUI frame

	state_manager.update(); // Update the current game state

	rlImGuiEnd(); // End Raylib-ImGUI frame

	EndDrawing(); // Tell Raylib we are done drawing

}

/**
 * @brief Check if the local client is hosting a server.
 * @return true if hosting, false otherwise.
 */
bool Game::is_hosting() {
	return client && server;
}