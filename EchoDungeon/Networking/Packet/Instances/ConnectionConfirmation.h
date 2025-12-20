#pragma once
#include "Networking/Packet/Packet.h"
#include "Networking/User/UserData.h"
#include <cereal/types/unordered_map.hpp>

// Packet type: 3
// Packet name: ConnectionConfirmation

/**
 * @brief Packet sent by server to accept a client's connection attempt and provide important information.
 */
class ConnectionConfirmationPacket : public Packet {
public:
	uint16_t client_assigned_id = 0; // Unique server-side ID assigned to the client

	// The idea is that a ConnectionConfirmationPacket will be sent alongside a
	//	UserDataUpdatePacket to provide the client with its assigned ID and the	peer datamap

	// Default constructor
	ConnectionConfirmationPacket()
		: Packet(3, true), client_assigned_id(0) {
	}

	// Constructor with data
	ConnectionConfirmationPacket(uint16_t _client_assigned_id)
		: Packet(3, true), client_assigned_id(_client_assigned_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ConnectionConfirmationPacket)
	PACKET_TO_ENET(ConnectionConfirmationPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, client_assigned_id);
	}
};
