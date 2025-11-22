#include "Host.h"
#include "Game/Game.h"
#include "Utils/UI.h"

Host::Host(Game& game) : GameState(game) {
	// Constructor
}

void Host::on_activate() {
	TRACE("Host activated");
}

void Host::on_deactivate() {
	TRACE("Host deactivated");
}

void Host::update() {
	UIUtils::FullscreenWindow([this]() {
		// Center and enlarge buttons
		ImVec2 buttonSize(200, 50);
		if (ImGui::Button("Back", buttonSize)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Host");
		ImGui::PopFont();
		});
}