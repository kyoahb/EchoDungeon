#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 4
// Packet name: DisconnectInfo

/**
 * @brief Packet sent by client to inform server of disconnection reason.
 */
class DisconnectInfoPacket : public Packet {
public:
	std::string disconnect_reason = "Disconnection requested: No further information provided";

	// Default constructor
	DisconnectInfoPacket()
		: Packet(4, true), disconnect_reason("") {
	}

	// Constructor with username
	DisconnectInfoPacket(const std::string& _disconnect_reason)
		: Packet(4, true), disconnect_reason(_disconnect_reason) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(DisconnectInfoPacket)
	PACKET_TO_ENET(DisconnectInfoPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, disconnect_reason);
	}
};
