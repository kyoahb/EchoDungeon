#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"

// Packet type: 15
// Packet name: PlayerInput

/**
 * @brief Client sends player transform to server.
 * Sent frequently (every frame or when input changes) with unreliable delivery.
 * Server validates and broadcasts the result via EntityUpdatePacket.
 */
class PlayerInputPacket : public Packet {
public:
	ObjectTransform transform;  // Player's desired transform

	// Default constructor (unreliable for performance)
	PlayerInputPacket()
		: Packet(15, false) {  // false = unreliable
	}

	// Constructor with data
	PlayerInputPacket(const ObjectTransform& _transform)
		: Packet(15, false), transform(_transform) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(PlayerInputPacket)
	PACKET_TO_ENET(PlayerInputPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, transform);
	}
};
