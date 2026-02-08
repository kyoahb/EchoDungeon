#include "Join.h"
#include "Game/Game.h"
#include "Utils/UI.h"

Join::Join(Game& game) : GameState(game) {
	// Constructor
}

void Join::on_activate() {
	TRACE("Join activated");
	connection_status = ConnectionStatus::IDLE;
	connection_future.reset();
	connection_error_message.clear();
}

void Join::on_deactivate() {
	TRACE("Join deactivated");
	connection_future.reset();
}

void Join::update() {
	static std::string errors = "";
	// Check connection status if we're connecting
	if (connection_status == ConnectionStatus::CONNECTING && connection_future) {
		// Check if the future is ready
		if (connection_future->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
			auto result = connection_future->get();
			if (result.success) {
				INFO("Connected to server successfully");
				connection_status = ConnectionStatus::CONNECTED;
				game.state_manager.set_state("Lobby");
			} else {
				ERROR("Failed to connect to server: " + result.failure_reason);
				connection_status = ConnectionStatus::FAILED;
				connection_error_message = result.failure_reason;
				game.client = nullptr;
			}
			connection_future.reset();
			return;
		}
	}

	UIUtils::FullscreenWindow([this]() {
		int should_block = connection_status == ConnectionStatus::CONNECTING;
		// Center and enlarge buttons
		ImVec2 button_size(200, 50);
		
		// Back button (disabled while connecting)
		if (should_block) {
			ImGui::BeginDisabled();
		}
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}
		if (should_block) {
			ImGui::EndDisabled();
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
			if (should_block) {
				ImGui::BeginDisabled();
			}
			UIUtils::InputText("", &ip_address);
			if (should_block) {
				ImGui::EndDisabled();
			}
		});

		UIUtils::YSpacing(20);

		// Port label and input box
		UIUtils::CentreText("Port");
		ImVec2 port_size(150, 20);
		static int port = NetworkConstants::DEFAULT_PORT;
		UIUtils::CentrePosition(port_size);
		UIUtils::WrapComponent("PortInput", port_size, [&]() {
			if (should_block) {
				ImGui::BeginDisabled();
			}
			ImGui::InputInt("", &port);
			if (should_block) {
				ImGui::EndDisabled();
			}
		});

		UIUtils::YSpacing(30);

		// Show connection status
		if (connection_status == ConnectionStatus::CONNECTING) {
			UIUtils::CentreText("Connecting...");
			UIUtils::YSpacing(10);
		} else if (connection_status == ConnectionStatus::FAILED) {
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
			UIUtils::CentreText("Connection failed: " + connection_error_message);
			ImGui::PopStyleColor();
			UIUtils::YSpacing(10);
		}

		UIUtils::CentrePosition(button_size);
		
		// Disable Join button while connecting
		if (should_block) {
			ImGui::BeginDisabled();
		}

		if (!errors.empty()) {
			// Show error popup
			UIUtils::CentreText(errors);
		}

		if (ImGui::Button("Join", button_size)) {
			// Validation

			auto is_valid_ip = [](const std::string& ip) {
				int parts[4];
				// Break ip up into 4 parts
				int matched = sscanf_s(ip.c_str(), "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]);
				if (matched != 4) return false;
				for (int i = 0; i < 4; i++) {
					// Validate each part
					if (parts[i] < 0 || parts[i] > 255) return false;
				}
				return true;
			};

			if (!is_valid_ip(ip_address)) {
				errors = "Invalid IP address format. Use X.X.X.X (0-255).\n";
				return;
			}
			if (port < 1 || port > 65535) {
				errors = "Port number must be between 1 and 65535.\n";
				return;
			}

			errors.clear(); // Clear errors on successful validation
			
			TRACE("Join button pressed with IP: " + ip_address + " Port: " + std::to_string(port));
			
			// Reset error message
			connection_error_message.clear();
			
			// Create client
			game.client = std::make_shared<Client>(game.settings.username);

			// Start connection asynchronously
			connection_status = ConnectionStatus::CONNECTING;
			auto future = game.client->connect(ip_address, static_cast<uint16_t>(port));
			connection_future = std::make_shared<std::future<ConnectionResult>>(std::move(future));
		}
		
		if (should_block) {
			ImGui::EndDisabled();
		}
		});
}