#pragma once
#include "Imports/common.h"

// Struct defining a running server
struct OpenServer {
	std::string address = ""; // Server address
	uint16_t port = 0;        // Server port
	bool closed = false;    // Is the server denying new connections
	std::string lobby_name = "Unnamed Lobby"; // Name of the server lobby
	uint8_t max_players = 16; // Maximum number of players allowed

	template<class Archive>
	void serialize(Archive& archive) {
		archive(address, port, closed, lobby_name, max_players);
	}
};