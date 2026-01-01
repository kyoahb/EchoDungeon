#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/EntityType.h"

// Packet type: 13
// Packet name: EntityDestroy

/**
 * @brief Destroys an entity (object or player) in the world.
 * Sent reliably when server removes an entity.
 */
class EntityDestroyPacket : public Packet {
public:
	uint16_t entity_id;  // ID of entity to destroy
	EntityType entity_type;  // Type of entity

	// Default constructor
	EntityDestroyPacket()
		: Packet(13, true), entity_id(0), entity_type(EntityType::OBJECT) {
	}

	// Constructor with data
	EntityDestroyPacket(uint16_t _entity_id, EntityType _entity_type)
		: Packet(13, true), entity_id(_entity_id), entity_type(_entity_type) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EntityDestroyPacket)
	PACKET_TO_ENET(EntityDestroyPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		uint8_t type_value = static_cast<uint8_t>(entity_type);
		archive(header, is_reliable, entity_id, type_value);
		entity_type = static_cast<EntityType>(type_value);
	}
};
