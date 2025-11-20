#include "Settings.h"
#include "Game/Game.h"
#include "Utils/UI.h"

Settings::Settings(Game& game) : GameState(game) {
	// Constructor
}

void Settings::on_activate() {
	TRACE("Settings activated");
}

void Settings::on_deactivate() {
	TRACE("Settings deactivated");
}

void Settings::update() {
	UIUtils::FullscreenWindow([this]() {
		// Center and enlarge buttons
		ImVec2 buttonSize(200, 50);
		if (ImGui::Button("Back", buttonSize)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Settings");
		ImGui::PopFont();
	});
}