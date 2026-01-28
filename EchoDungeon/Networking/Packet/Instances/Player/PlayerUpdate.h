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
	float health = 100.0f;              // Current health

	template<class Archive>
	void serialize(Archive& archive) {
		archive(id, transform, health);
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
