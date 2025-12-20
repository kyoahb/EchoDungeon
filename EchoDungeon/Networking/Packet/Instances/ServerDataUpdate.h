#pragma once
#include "Networking/Packet/Packet.h"
#include "Networking/User/UserData.h"
#include <cereal/types/unordered_map.hpp>
#include "Networking/Server/OpenServer.h"

// Packet type: 6
// Packet name: ServerDataUpdate

/**
 * @brief Packet sent by server to inform client of an update to user data map.
 */
class ServerDataUpdatePacket : public Packet {
public:
	std::unordered_map<uint16_t, UserData> current_peers; // Current list of connected peers
	OpenServer server_info; // Information about the running server.

	// Default constructor
	ServerDataUpdatePacket()
		: Packet(6, true), current_peers({}), server_info() {
	}

	// Constructor with data
	ServerDataUpdatePacket(const std::unordered_map<uint16_t, UserData>& _current_peers, const OpenServer& _server_info)
		: Packet(6, true), current_peers(_current_peers), server_info(_server_info) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ServerDataUpdatePacket)
	PACKET_TO_ENET(ServerDataUpdatePacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, current_peers, server_info);
	}
};
