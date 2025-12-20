#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 1
// Packet name: ConnectionInitiation

/**
 * @brief Packet sent by client to initiate connection to server.
 */
class ConnectionInitiationPacket : public Packet {
public:
	std::string client_preferred_username;

	// Default constructor
	ConnectionInitiationPacket() 
		: Packet(1, true), client_preferred_username("") {}
	
	// Constructor with username
	ConnectionInitiationPacket(const std::string& username)
		: Packet(1, true), client_preferred_username(username) {}

	// Macros for serialization
	PACKET_DESERIALIZE(ConnectionInitiationPacket)
	PACKET_TO_ENET(ConnectionInitiationPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, client_preferred_username);
	}
};
