#include "MainMenu.h"
#include "Game/Game.h"

MainMenu::MainMenu(Game& game) : GameState(game) {
	// Constructor
}

void MainMenu::on_activate() {
	TRACE("MainMenu activated");
}

void MainMenu::on_deactivate() {
	TRACE("MainMenu deactivated");
}

void MainMenu::update() {
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() });

	// Simple main menu UI
	ImGui::Begin("FullscreenWindow", NULL,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	ImGui::Text("Welcome to Echo Dungeon!");
	if (ImGui::Button("Start Game")) {
		TRACE("Start Game button pressed");
		// Here you would typically switch to the game state
		// game.state_manager.set_state("GameplayState");
	}
	if (ImGui::Button("Exit")) {
		TRACE("Exit button pressed");
	}
	ImGui::End();
}