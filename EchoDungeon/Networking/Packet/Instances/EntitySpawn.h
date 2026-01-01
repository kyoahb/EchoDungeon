#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/Object.h"
#include <cereal/types/string.hpp>

// Packet type: 12
// Packet name: EntitySpawn

/**
 * @brief Spawns a new entity (object) in the world.
 * Sent reliably when server creates a new object.
 */
class EntitySpawnPacket : public Packet {
public:
	Object object;  // The full object to spawn

	// Default constructor
	EntitySpawnPacket()
		: Packet(12, true) {
	}

	// Constructor with data
	EntitySpawnPacket(const Object& _object)
		: Packet(12, true), object(_object) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EntitySpawnPacket)
	PACKET_TO_ENET(EntitySpawnPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, object);
	}
};
