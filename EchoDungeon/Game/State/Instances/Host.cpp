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
		ImVec2 button_size(200, 50);
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Host");
		ImGui::PopFont();

		// Lobby name label and input box
		UIUtils::CentreText("Lobby name");
		ImVec2 lobbyname_size(150, 20);
		static std::string lobby_name = "Lobby";
		UIUtils::CentrePosition(lobbyname_size);
		ImGui::PushItemWidth(lobbyname_size.x);
		ImGui::PushID("LobbyNameInput");
		UIUtils::InputText("", &lobby_name);
		ImGui::PopID();
		ImGui::PopItemWidth();

		// Max players label and input box
		UIUtils::CentreText("Max players");
		ImVec2 maxplayers_size(150, 20);
		static int max_players = 25565;
		UIUtils::CentrePosition(maxplayers_size);
		ImGui::PushItemWidth(maxplayers_size.x);
		ImGui::PushID("PortInput");
		ImGui::InputInt("", &max_players);
		ImGui::PopID();
		ImGui::PopItemWidth();

		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Host", button_size)) {
			TRACE("Host confirmation pressed");

			// Print out all information as a test
			TRACE(" - Lobby name: " + lobby_name);
			TRACE(" - Max players: " + std::to_string(max_players));
		}
		});
}