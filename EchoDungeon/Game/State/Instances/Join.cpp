#include "Join.h"
#include "Game/Game.h"
#include "Utils/UI.h"

Join::Join(Game& game) : GameState(game) {
	// Constructor
}

void Join::on_activate() {
	TRACE("Join activated");
}

void Join::on_deactivate() {
	TRACE("Join deactivated");
}

void Join::update() {
	UIUtils::FullscreenWindow([this]() {
		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Join");
		ImGui::PopFont();

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 100); // Add some vertical spacing

		// Center and enlarge buttons
		ImVec2 buttonSize(200, 50);
		UIUtils::CentrePosition(buttonSize);
		if (ImGui::Button("Back", buttonSize)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}
		});
}