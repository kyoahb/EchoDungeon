#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Game/World/Entities/Enemy.h"
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

// Packet type: 11
// Packet name: WorldSnapshot

/**
 * @brief Full world state snapshot sent by server to client.
 * Used for initial synchronization when joining, or forced resync.
 */
class WorldSnapshotPacket : public Packet {
public:
	std::unordered_map<uint32_t, Player> players; // All players in the world
	std::unordered_map<uint32_t, Object> objects; // All objects in the world (keyed by object ID)
	std::unordered_map<uint32_t, Enemy> enemies; // All enemies in the world (keyed by enemy ID)

	// Default constructor
	WorldSnapshotPacket()
		: Packet(11, true) {
	}

	// Constructor with data
	WorldSnapshotPacket(
		const std::unordered_map<uint32_t, Player>& _players,
		const std::unordered_map<uint32_t, Object>& _objects,
		const std::unordered_map<uint32_t, Enemy>& _enemies
	)
		: Packet(11, true), players(_players), objects(_objects), enemies(_enemies) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(WorldSnapshotPacket)
	PACKET_TO_ENET(WorldSnapshotPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, players, objects);
	}
};
