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
}

void Lobby::on_deactivate() {
	TRACE("Lobby deactivated");
}

void Lobby::update() {
	UIUtils::FullscreenWindow([this]() {
		ImVec2 button_size(200, 50);
		
		// Check if local player is host
		bool is_host = false;
		auto local_peer = game.client->peers.get_local_peer();
		if (local_peer.has_value()) {
			is_host = local_peer->is_host;
		}

		// Top row: Back button on left, Start button on right (if host)
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed on lobby, attempting reset");

			// Destroy server if it exists
			if (game.server) {
				game.server->stop();
				game.server = nullptr;
			}

			// Destroy client if it exists
			if (game.client) {
				game.client->disconnect().get();
				game.client->stop();
				game.client = nullptr;
			}

			game.state_manager.set_state("MainMenu");
		}

		// Start button on the right (host only)
		if (is_host) {
			ImGui::SameLine(ImGui::GetWindowWidth() - button_size.x - 20);
			if (ImGui::Button("Start", button_size)) {
				TRACE("Start button pressed");
				// TODO: Handle start game
			}
		}

		UIUtils::YSpacing(20);

		// Lobby info row: [Lobby Name] ... [Num Players / Max Players] ... [Allow Connections checkbox if host]
		auto& server_info = game.client->connected_server_info;
		auto all_peers = game.client->peers.get_all_peers();
		
		ImGui::Text("Lobby: %s", server_info.lobby_name.c_str());
		
		ImGui::SameLine(ImGui::GetWindowWidth() / 2 - 50);
		ImGui::Text("Players: %zu / %d", all_peers.size(), server_info.max_players);

		// Allow new connections checkbox (host only)
		if (is_host) {
			ImGui::SameLine(ImGui::GetWindowWidth() - 200);
			static bool allow_connections = !server_info.closed;
			if (ImGui::Checkbox("Allow new connections", &allow_connections)) {
				// TODO: Handle allow connections toggle
			}
		}

		UIUtils::YSpacing(20);

		// Player table
		ImGuiTableFlags table_flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;
		
		int column_count = is_host ? 4 : 3; // Add action column for host
		
		if (ImGui::BeginTable("PlayersTable", column_count, table_flags)) {
			// Setup columns
			ImGui::TableSetupColumn("Player Name", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Player ID", ImGuiTableColumnFlags_WidthFixed, 80.0f);
			ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, 120.0f);
			if (is_host) {
				ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed, 80.0f);
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
							// TODO: Handle kick
						}
						ImGui::PopID();
					}
				}
			}

			ImGui::EndTable();
		}

	});
}