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
		// Center and enlarge buttons
		ImVec2 button_size(200, 50);
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Join");
		ImGui::PopFont();

		UIUtils::YSpacing(30);

		// IP address label and input box
		UIUtils::CentreText("IP Address");
		ImVec2 ipaddress_size(200, 20);
		static std::string ip_address = "127.0.0.1";
		UIUtils::CentrePosition(ipaddress_size);
		UIUtils::WrapComponent("IPAddressInput", ipaddress_size, [&]() {
			UIUtils::InputText("", &ip_address);
		});

		UIUtils::YSpacing(20);

		// Port label and input box
		UIUtils::CentreText("Port");
		ImVec2 port_size(150, 20);
		static int port = NetworkConstants::DEFAULT_PORT;
		UIUtils::CentrePosition(port_size);
		UIUtils::WrapComponent("PortInput", port_size, [&]() {
			ImGui::InputInt("", &port);
		});

		UIUtils::YSpacing(30);

		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Join", button_size)) {
			TRACE("Join button pressed with IP: " + ip_address + " Port: " + std::to_string(port));
			// Create client
			game.client = std::make_shared<Client>(game.settings.username);

			// Attempt to connect
			auto result = game.client->connect(ip_address, static_cast<uint16_t>(port)).get();
			if (result.success) {
				INFO("Connected to server successfully");
				game.state_manager.set_state("Lobby");
			} else {
				ERROR("Failed to connect to server: " + result.failure_reason);
				game.client = nullptr;
			}
		}
		});
}