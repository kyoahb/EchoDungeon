#include "Game.h"

Game::Game() {

	Logger::init(); // Initialise logger first

	// Setup raylib window
	const int screenWidth = 720;
	const int screenHeight = 480;

	window = raylib::Window(screenWidth, screenHeight, "Echo Dungeon");
	rlImGuiSetup(true); // Setup Raylib ImGUI connection

	INFO("Game initialised");
}

Game::~Game() {
	rlImGuiShutdown(); // Shutdown Raylib ImGUI connection
	INFO("Game de-initialised");
}

void Game::begin() {
	INFO("Game started");
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

	auto a = raylib::Text("Example text rendered to the screen.", 20, BLACK);

	rlImGuiEnd(); // End Raylib-ImGUI frame

	EndDrawing(); // Tell Raylib we are done drawing

}