#include "Lobby.h"
#include "Game/Game.h"
#include "Utils/UI.h"

Lobby::Lobby(Game& game) : GameState(game) {
	// Constructor
}

void Lobby::on_activate() {
	TRACE("Lobby activated");

	if (!game.client) {
		ERROR("Lobby activated but no client exists?");
		return;
	}

	on_disconnect_callback = ClientEvents::DisconnectEvent::register_callback(
		[this](const ClientEvents::DisconnectEventData& data) {
			handle_disconnect_event(data);
		}
	);
}

void Lobby::on_deactivate() {
	TRACE("Lobby deactivated");

	// Unregister disconnect callback
	ClientEvents::DisconnectEvent::unregister_callback(on_disconnect_callback);
}

void Lobby::handle_disconnect_event(const ClientEvents::DisconnectEventData& data) {
	TRACE("Handling disconnect event in lobby state, setting quit to main menu flag");
	should_quit_to_mainmenu.store(true);
}

void Lobby::update() {
	if (should_quit_to_mainmenu.load()) {
		TRACE("Disconnect detected in lobby state, returning to main menu");
		should_quit_to_mainmenu.store(false);

		// Destroy server if it exists
		if (game.server) {
			game.server->stop().get();
			game.server = nullptr;
		}

		// Destroy client if it exists
		if (game.client) {
			game.client->disconnect("Client pressed 'back' button on lobby").get();
			game.client->stop();
			game.client = nullptr;
		}

		game.state_manager.set_state("MainMenu");
		return;
	}
	UIUtils::FullscreenWindow([this]() {
		ImVec2 button_size(200, 50);

		// Check if local player is host
		bool is_host = false;
		auto local_peer = game.client->peers.get_local_peer();
		if (local_peer.has_value() && game.server != nullptr) {
			is_host = local_peer->is_host;
		}

		// Top row: Back button on left, Start button on right (if host)
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed on lobby, setting quit to main menu flag");
			should_quit_to_mainmenu.store(true);
		}

		// Start button on the right (host only)
		if (is_host) {
			ImGui::SameLine(ImGui::GetWindowWidth() - button_size.x - 20);
			if (ImGui::Button("Start", button_size)) {
				TRACE("Start button pressed");
				// TODO: Handle start game
			}
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Lobby");
		ImGui::PopFont();

		UIUtils::YSpacing(20);

		// Lobby info row: [Lobby Name] ... [Num Players / Max Players] ... [Allow Connections checkbox if host]
		auto& server_info = game.client->connected_server_info;
		auto all_peers = game.client->peers.get_all_peers();
		
		// Push smaller font for info row
		ImGui::PushFont(NULL, 17.0f);
		
		// Lobby name
		ImGui::Text("Name: %s", server_info.lobby_name.c_str());
		
		// Number of players
		ImGui::SameLine(ImGui::GetWindowWidth() * 0.25f);
		ImGui::Text("Players: %zu / %d", all_peers.size(), server_info.max_players);

		// Ip and port we are connected to
		ImGui::SameLine(ImGui::GetWindowWidth() * 0.5f);
		ImGui::Text("%s:%d", server_info.external_address.c_str(), server_info.port);

		// Allow new connections checkbox (host only)
		if (is_host) {
			ImGui::SameLine(ImGui::GetWindowWidth() * 0.75f);
			static bool allow_connections = !server_info.closed;
			if (ImGui::Checkbox("Allow new connections", &allow_connections)) {
				// TODO: Handle allow connections toggle
			}
		}
		
		// Pop font
		ImGui::PopFont();

		UIUtils::YSpacing(20);

		// Player table
		ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
		
		int column_count = is_host ? 4 : 3; // Add action column for host
		
		if (ImGui::BeginTable("PlayersTable", column_count, table_flags)) {
			// Setup columns
			ImGui::TableSetupColumn("Player Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 250.0f);
			ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 250.0f);
			if (is_host) {
				ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 100.0f);
			}
			ImGui::TableHeadersRow();

			// Draw each player row
			for (const auto& peer : all_peers) {
				ImGui::TableNextRow();
				
				// Player Name
				ImGui::TableNextColumn();
				ImGui::Text("%s", peer.username.c_str());

				// Player ID
				ImGui::TableNextColumn();
				ImGui::Text("%d", peer.server_side_id);

				// Status
				ImGui::TableNextColumn();
				bool is_local = game.client->peers.is_local(peer);
				bool is_peer_host = peer.is_host;
				
				if (is_local && is_peer_host) {
					ImGui::Text("YOU (Host)");
				} else if (is_local) {
					ImGui::Text("YOU");
				} else if (is_peer_host) {
					ImGui::Text("Host");
				} else {
					ImGui::Text("Player");
				}

				// Action column (host only)
				if (is_host) {
					ImGui::TableNextColumn();
					if (!is_local) {
						ImGui::PushID(peer.server_side_id); // Unique ID for each button
						if (ImGui::SmallButton("Kick")) {
							TRACE("Kick button pressed for player: " + peer.username);
							game.server->disconnect_peer(peer.server_side_id, "Kicked by host");
							
						}
						ImGui::PopID();
					}
				}
			}

			ImGui::EndTable();
		}

	});
}