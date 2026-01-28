#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include <cereal/types/string.hpp>

// Packet type: 14
// Packet name: PlayerSpawn

/**
 * @brief Spawns a new player in the world.
 * Sent reliably when a player joins the game world.
 * Contains only the essential data needed to create a player.
 */
class PlayerSpawnPacket : public Packet {
public:
	uint32_t id = 0;                    // Player ID (peer_id)
	std::string name = "Unknown";       // Player name
	ObjectTransform transform;          // Initial transform
	float health = 100.0f;              // Current health
	float max_health = 100.0f;          // Maximum health
	float damage = 10.0f;               // Damage dealt
	float range = 2.0f;                 // Attack range
	float speed = 2.0f;                 // Movement speed
	std::string asset_id = "player";    // Asset reference

	// Default constructor
	PlayerSpawnPacket()
		: Packet(14, true) {
	}

	// Constructor with data
	PlayerSpawnPacket(uint32_t _id, const std::string& _name, const ObjectTransform& _transform,
		float _health = 100.0f, float _max_health = 100.0f, float _damage = 10.0f,
		float _range = 2.0f, float _speed = 2.0f, const std::string& _asset_id = "player")
		: Packet(14, true), id(_id), name(_name), transform(_transform),
		health(_health), max_health(_max_health), damage(_damage),
		range(_range), speed(_speed), asset_id(_asset_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(PlayerSpawnPacket)
	PACKET_TO_ENET(PlayerSpawnPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, id, name, transform, health, max_health, damage, range, speed, asset_id);
	}
};
