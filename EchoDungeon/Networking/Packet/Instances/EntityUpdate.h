#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include "Game/World/Entities/EntityType.h"

// Packet type: 11
// Packet name: EntityUpdate

/**
 * @brief Lightweight entity update for position/rotation/health changes.
 * Sent frequently (every tick) from server to all clients.
 * Includes players, NPCs, and dynamic objects.
 */

struct EntityUpdateData {
uint32_t entity_id;  // ID
EntityType entity_type = EntityType::OBJECT;
ObjectTransform transform;
float health = 100.0f; // Player health
float damage = 10.0f; // Damage dealt
float max_health = 100.0f; // Maximum health
float range = 2.0f; // Attack range
float speed = 2.0f; // Movement speed

	template<class Archive>
	void serialize(Archive& archive) {
		uint8_t type_value = static_cast<uint8_t>(entity_type);
		archive(entity_id, type_value, transform, health);
		entity_type = static_cast<EntityType>(type_value);
	}
};

class EntityUpdatePacket : public Packet {
public:
	std::vector<EntityUpdateData> updates; // List of entity updates

	// Default constructor (unreliable for performance)
	EntityUpdatePacket()
		: Packet(11, false) {  // false = unreliable
	}

	// Constructor with data
	EntityUpdatePacket(const std::vector<EntityUpdateData>& _updates)
		: Packet(11, false), updates(_updates) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EntityUpdatePacket)
	PACKET_TO_ENET(EntityUpdatePacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, updates);
	}
};
