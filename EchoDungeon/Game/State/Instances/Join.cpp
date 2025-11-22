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

		// IP address label and input box
		UIUtils::CentreText("IP Address");
		ImVec2 ipaddress_size(150, 20);
		static std::string ip_address = "255.255.255.255"; // Contains placeholder text
		UIUtils::CentrePosition(ipaddress_size);
		ImGui::PushItemWidth(ipaddress_size.x);
		ImGui::PushID("IPAddressInput");
		UIUtils::InputText("", &ip_address);
		ImGui::PopID();
		ImGui::PopItemWidth();

		// Port label and input box
		UIUtils::CentreText("Port");
		ImVec2 port_size(150, 20);
		static int port = 25565;
		UIUtils::CentrePosition(port_size);
		ImGui::PushItemWidth(port_size.x);
		ImGui::PushID("PortInput");
		ImGui::InputInt("", &port);
		ImGui::PopID();
		ImGui::PopItemWidth();

		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Join", button_size)) {
			TRACE("Join confirmation pressed");

			// Print out all information as a test
			TRACE(" - IP: " + ip_address);
			TRACE(" - Port: " + std::to_string(port));
		}
		});
}