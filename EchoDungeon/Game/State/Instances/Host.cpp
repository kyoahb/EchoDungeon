#include "Host.h"
#include "Game/Game.h"
#include "Utils/UI.h"
#include "Networking/Server/Server.h"
#include "Networking/Client/Client.h"
#include "Networking/NetworkConstants.h"

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
	static std::string errors = ""; // Persistent error message across frames

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

		UIUtils::YSpacing(30);

		// Lobby name label and input box
		UIUtils::CentreText("Lobby name");
		ImVec2 lobbyname_size(150, 20);
		static std::string lobby_name = "Lobby";
		UIUtils::CentrePosition(lobbyname_size);
		UIUtils::WrapComponent("LobbyNameInput", lobbyname_size, [&]() {
			UIUtils::InputText("", &lobby_name);
		});

		UIUtils::YSpacing(20);

		// Max players label and input box
		UIUtils::CentreText("Max players");
		ImVec2 maxplayers_size(100, 20);
		static int max_players = 4; // Reasonable default of 4 players
		UIUtils::CentrePosition(maxplayers_size);
		UIUtils::WrapComponent("MaxPlayersInput", maxplayers_size, [&]() {
			ImGui::InputInt("", &max_players);
		});

		UIUtils::YSpacing(30);

		if (!errors.empty()) {
			// Show error popup
			UIUtils::CentreText(errors);
		}

		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Host", button_size)) {

			// Validation
			if (lobby_name.empty()) {
				errors = "Lobby name cannot be empty.\n";
				return;
			}
			if (max_players < 1) {
				errors = "Max players must be greater than 0.\n";
				return;
			}
			errors.clear();


			TRACE("Host button pressed with Lobby Name: " + lobby_name + " Max Players: " + std::to_string(max_players));
			// Create server
			game.server = std::make_shared<Server>("0.0.0.0", NetworkConstants::DEFAULT_PORT);
			game.server->server_info.lobby_name = lobby_name;
			game.server->server_info.max_players = max_players;
			// Address 0.0.0.0 binds to all interfaces, allowing access from
			//		- localhost / 127.0.0.1
			//		- local network IP (e.g., 192.168.x.x)
			//		- public IP (if port forwarding is set up)

			// Start server networking loop
			game.server->start();

			// Create host client
			game.client = std::make_shared<Client>(game.settings.username);
			// Attempt to connect to local server
			auto result = game.client->connect("127.0.0.1", NetworkConstants::DEFAULT_PORT).get();
			if (result.success) {
				INFO("Host client connected successfully");
				game.state_manager.set_state("Lobby");
			} else {
				ERROR("Host client failed to connect: " + result.failure_reason);
				// Cleanup on failure
				game.server->stop();
				game.server = nullptr;
				game.client = nullptr;
			}

		}
		});
}