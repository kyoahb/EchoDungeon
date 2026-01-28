#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 21
// Packet name: ObjectDestroy

/**
 * @brief Destroys an object in the world.
 * Sent reliably when an object is removed from the world.
 */
class ObjectDestroyPacket : public Packet {
public:
	uint32_t id = 0;  // Object ID to destroy

	// Default constructor
	ObjectDestroyPacket()
		: Packet(21, true) {
	}

	// Constructor with data
	ObjectDestroyPacket(uint32_t _id)
		: Packet(21, true), id(_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ObjectDestroyPacket)
	PACKET_TO_ENET(ObjectDestroyPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, id);
	}
};
