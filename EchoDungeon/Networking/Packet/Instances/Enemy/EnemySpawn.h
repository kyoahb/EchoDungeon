#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include <cereal/types/string.hpp>

// Packet type: 17
// Packet name: EnemySpawn

/**
 * @brief Spawns a new enemy in the world.
 * Sent reliably when server creates a new enemy.
 * Contains only the essential data needed to create an enemy.
 */
class EnemySpawnPacket : public Packet {
public:
	uint32_t id = 0;                    // Enemy ID
	ObjectTransform transform;          // Initial transform
	float health = 100.0f;              // Current health
	float max_health = 100.0f;          // Maximum health
	float damage = 10.0f;               // Damage dealt
	float speed = 1.0f;                 // Movement speed
	bool spawns_items = true;         // Does this enemy spawn items on death?
	std::string asset_id = "zombie";    // Asset reference

	// Default constructor
	EnemySpawnPacket()
		: Packet(17, true) {
	}

	// Constructor with data
	EnemySpawnPacket(uint32_t _id, const ObjectTransform& _transform,
		float _health, float _max_health, float _damage, float _speed,
		bool _spawns_items = true,
		const std::string& _asset_id = "zombie")
		: Packet(17, true), id(_id), transform(_transform),
		health(_health), max_health(_max_health), damage(_damage),
		speed(_speed), spawns_items(_spawns_items), asset_id(_asset_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EnemySpawnPacket)
	PACKET_TO_ENET(EnemySpawnPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, id, transform, health, max_health, damage, speed, spawns_items, asset_id);
	}
};
