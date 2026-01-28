#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 16
// Packet name: PlayerDestroy

/**
 * @brief Destroys a player in the world.
 * Sent reliably when a player disconnects or is removed.
 */
class PlayerDestroyPacket : public Packet {
public:
	uint32_t id = 0;  // Player ID to destroy

	// Default constructor
	PlayerDestroyPacket()
		: Packet(16, true) {
	}

	// Constructor with data
	PlayerDestroyPacket(uint32_t _id)
		: Packet(16, true), id(_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(PlayerDestroyPacket)
	PACKET_TO_ENET(PlayerDestroyPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, id);
	}
};
