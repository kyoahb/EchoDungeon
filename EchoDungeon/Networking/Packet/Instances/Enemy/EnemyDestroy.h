#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 19
// Packet name: EnemyDestroy

/**
 * @brief Destroys an enemy in the world.
 * Sent reliably when an enemy is killed or removed.
 */
class EnemyDestroyPacket : public Packet {
public:
	uint32_t id = 0;  // Enemy ID to destroy

	// Default constructor
	EnemyDestroyPacket()
		: Packet(19, true) {
	}

	// Constructor with data
	EnemyDestroyPacket(uint32_t _id)
		: Packet(19, true), id(_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EnemyDestroyPacket)
	PACKET_TO_ENET(EnemyDestroyPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, id);
	}
};
