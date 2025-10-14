#include "Game.h"

Game::Game() {

	Logger::init(); // Initialise logger first

	// Setup raylib window
	const int screenWidth = 720;
	const int screenHeight = 480;

	window = raylib::Window(screenWidth, screenHeight, "Echo Dungeon");
	rlImGuiSetup(true); // Setup Raylib ImGUI connection

	TRACE("Game initialised");
}

Game::~Game() {
	rlImGuiShutdown(); // Shutdown Raylib ImGUI connection
	TRACE("Game de-initialised");
}

void Game::begin() {
	TRACE("Game started");
	while (!window.ShouldClose()) {
		update();
	}
}

void Game::update() {
	// Execute pre-frame updates

	// Draw
	BeginDrawing(); // Tell Raylib we are going to draw

	window.ClearBackground(GRAY); // Clear previous frame by replacing with full white
	rlImGuiBegin(); // Start Raylib-ImGUI frame

	auto exampleText = raylib::Text("Example text rendered to the screen.", 20, BLACK);
	exampleText.Draw(10, 10);

	rlImGuiEnd(); // End Raylib-ImGUI frame

	EndDrawing(); // Tell Raylib we are done drawing

}