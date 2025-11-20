#include "MainMenu.h"
#include "Game/Game.h"
#include "Utils/UI.h"

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
	UIUtils::FullscreenWindow([this]() {
		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Echo Dungeon");
		ImGui::PopFont();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100); // Add some vertical spacing

		// Center and enlarge buttons
		ImVec2 buttonSize(200, 50);
		UIUtils::CentrePosition(buttonSize);
		if (ImGui::Button("Host", buttonSize)) {
			TRACE("Host button pressed");
			game.state_manager.set_state("Host");
		}
		UIUtils::CentrePosition(buttonSize);
		if (ImGui::Button("Join", buttonSize)) {
			TRACE("Join button pressed");
			game.state_manager.set_state("Join");
		}
		UIUtils::CentrePosition(buttonSize);
		if (ImGui::Button("Settings", buttonSize)) {
			TRACE("Settings button pressed");
			game.state_manager.set_state("Settings");
		}
	});
}