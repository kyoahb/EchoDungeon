#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 5
// Packet name: DisconnectKick

/**
 * @brief Packet sent by server to inform client that it is being kicked and the reason why.
 */
class DisconnectKickPacket : public Packet {
public:
	std::string disconnect_reason = "Disconnection requested: No further information provided";

	// Default constructor
	DisconnectKickPacket()
		: Packet(5, true), disconnect_reason("") {
	}

	// Constructor with username
	DisconnectKickPacket(const std::string& _disconnect_reason)
		: Packet(5, true), disconnect_reason(_disconnect_reason) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(DisconnectKickPacket)
	PACKET_TO_ENET(DisconnectKickPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, disconnect_reason);
	}
};
