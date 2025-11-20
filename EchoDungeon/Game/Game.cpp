#include "Game.h"
#include "Game/State/Instances/MainMenu.h"
#include "Game/State/Instances/Settings.h"
#include "Game/State/Instances/Host.h"
#include "Game/State/Instances/Join.h"

Game::Game() : state_manager(GameStateManager()){

	Logger::init(); // Initialise logger first

	// Setup raylib window
	const int screenWidth = 720;
	const int screenHeight = 480;

	window = raylib::Window(screenWidth, screenHeight, "Echo Dungeon");
	rlImGuiSetup(true); // Setup Raylib ImGUI connection


	// Set default font to Arial, size 24
	ImGuiIO& io = ImGui::GetIO();
	ImFont* arialFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/arial.ttf", 24.0f);
	io.FontDefault = arialFont; // Set as default for larger text

	// Add all GameStates here
	state_manager.add_state("MainMenu", std::make_shared<MainMenu>(*this));
	state_manager.add_state("Settings", std::make_shared<Settings>(*this));
	state_manager.add_state("Host", std::make_shared<Host>(*this));
	state_manager.add_state("Join", std::make_shared<Join>(*this));

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
	// Execute pre-frame updates

	// Draw
	BeginDrawing(); // Tell Raylib we are going to draw

	window.ClearBackground(GRAY); // Clear previous frame by replacing with full white
	rlImGuiBegin(); // Start Raylib-ImGUI frame

	state_manager.update(); // Update the current game state

	rlImGuiEnd(); // End Raylib-ImGUI frame

	EndDrawing(); // Tell Raylib we are done drawing

}