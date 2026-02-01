#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include <cereal/types/vector.hpp>

// Packet type: 15
// Packet name: PlayerUpdate

/**
 * @brief Lightweight player update packet for frequent state synchronization.
 * Sent frequently (every tick) from server to all clients.
 * Contains only the data that changes frequently.
 */
struct PlayerUpdateData {
	uint32_t id = 0;                    // Player ID
	ObjectTransform transform;          // Current transform
	float health = 100.0f; // Player health
	float damage = 10.0f; // Damage dealt by the player
	float max_health = 100.0f; // Maximum health
	float range = 2.0f; // Attack range
	float speed = 2.0f; // Units per second 
	uint64_t attack_cooldown = 500; // Milliseconds between attacks

	uint64_t last_attack_time = 0; // Timestamp of last attack (milliseconds)
	bool attacking = false; // Is the player currently attacking?

	template<class Archive>
	void serialize(Archive& archive) {
		archive(id, transform, health, damage, max_health, range, speed, attack_cooldown, last_attack_time, attacking);
	}
};

class PlayerUpdatePacket : public Packet {
public:
	std::vector<PlayerUpdateData> updates; // List of player updates

	// Default constructor (reliable for now, but can be made unreliable for performance)
	PlayerUpdatePacket()
		: Packet(15, true) {
	}

	// Constructor with data
	PlayerUpdatePacket(const std::vector<PlayerUpdateData>& _updates)
		: Packet(15, true), updates(_updates) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(PlayerUpdatePacket)
	PACKET_TO_ENET(PlayerUpdatePacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, updates);
	}
};
