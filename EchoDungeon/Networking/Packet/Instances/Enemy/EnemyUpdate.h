#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/ObjectTransform.h"
#include <cereal/types/vector.hpp>

// Packet type: 18
// Packet name: EnemyUpdate

/**
 * @brief Lightweight enemy update packet for frequent state synchronization.
 * Sent frequently (every tick) from server to all clients.
 * Contains only the data that changes frequently.
 */
struct EnemyUpdateData {
	uint32_t id = 0;                    // Enemy ID
	ObjectTransform transform;          // Current transform
	float health = 100.0f;              // Current health

	template<class Archive>
	void serialize(Archive& archive) {
		archive(id, transform, health);
	}
};

class EnemyUpdatePacket : public Packet {
public:
	std::vector<EnemyUpdateData> updates; // List of enemy updates

	// Default constructor (reliable for now, but can be made unreliable for performance)
	EnemyUpdatePacket()
		: Packet(18, true) {
	}

	// Constructor with data
	EnemyUpdatePacket(const std::vector<EnemyUpdateData>& _updates)
		: Packet(18, true), updates(_updates) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(EnemyUpdatePacket)
	PACKET_TO_ENET(EnemyUpdatePacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, updates);
	}
};
