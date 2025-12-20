#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 2
// Packet name: ConnectionRefusal

/**
 * @brief Packet sent by server to refuse a client's connection attempt.
 */
class ConnectionRefusalPacket : public Packet {
public:
	std::string refusal_reason = "Connection refused: No further information provided";

	// Default constructor
	ConnectionRefusalPacket()
		: Packet(2, true), refusal_reason("") {
	}

	// Constructor with username
	ConnectionRefusalPacket(const std::string& _refusal_reason)
		: Packet(2, true), refusal_reason(_refusal_reason) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ConnectionRefusalPacket)
	PACKET_TO_ENET(ConnectionRefusalPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, refusal_reason);
	}
};
