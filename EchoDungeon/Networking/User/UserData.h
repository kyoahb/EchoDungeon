#pragma once
#include "Imports/common.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

enum class UserStatus : uint8_t {
	DISCONNECTED = 0,
	CONNECTING = 1,
	CONNECTED = 2
};

struct UserData {
uint32_t server_side_id = 0; // Server-side user ID
std::string username = "Unknown"; // Username of the user
	bool is_host = false; // Is the user the host (hosting users are also running the server locally)
	std::string ip_address = "???.???.???.???"; // IP address of the user
	uint64_t connected_at = 0; // Timestamp of when the user connected (ms since epoch)
	uint64_t last_packet_time = 0; // Timestamp of the last packet received from this user (ms since epoch)
	UserStatus status = UserStatus::DISCONNECTED; // Current status of the user
	std::string current_state = "Unknown"; // Current state the user is in

	template<class Archive>
	void serialize(Archive& archive) {
		archive(server_side_id, username, is_host, ip_address, connected_at, last_packet_time, status, current_state);
	}
};