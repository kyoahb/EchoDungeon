#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include "Game/World/Entities/Object.h"
#include <cereal/types/string.hpp>

// Packet type: 20
// Packet name: ObjectSpawn

/**
 * @brief Spawns a new object in the world.
 * Sent reliably when server creates a new object.
 * Contains only the essential data needed to create an object.
 * Objects are static and don't need updates, only spawn and destroy.
 */
class ObjectSpawnPacket : public Packet {
public:
	uint32_t id = 0;                         // Object ID
	std::string asset_id = "";               // Asset reference
	ObjectType object_type = ObjectType::MODEL;  // Type of object (MODEL or IMAGE_MODEL)
	ObjectTransform transform;               // Transform (position, rotation, scale)
	raylib::Color color = raylib::Color::White(); // Color tint (default white)

	// Default constructor
	ObjectSpawnPacket()
		: Packet(20, true) {
	}

	// Constructor with data
	ObjectSpawnPacket(uint32_t _id, const std::string& _asset_id,
		ObjectType _object_type, const ObjectTransform& _transform, raylib::Color _color = raylib::Color::White())
		: Packet(20, true), id(_id), asset_id(_asset_id),
		object_type(_object_type), transform(_transform), color(_color) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ObjectSpawnPacket)
	PACKET_TO_ENET(ObjectSpawnPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		uint8_t type_value = static_cast<uint8_t>(object_type);
		archive(header, is_reliable, id, asset_id, type_value, transform, color);
		object_type = static_cast<ObjectType>(type_value);
	}
};
