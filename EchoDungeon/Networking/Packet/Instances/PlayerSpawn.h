#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/Player.h"
#include <cereal/types/string.hpp>

// Packet type: 14
// Packet name: PlayerSpawn

/**
 * @brief Spawns a new player in the world.
 * Sent reliably when a player joins the game world.
 */
class PlayerSpawnPacket : public Packet {
public:
	Player player;  // The full player to spawn

	// Default constructor
	PlayerSpawnPacket()
		: Packet(14, true) {
	}

	// Constructor with data
	PlayerSpawnPacket(const Player& _player)
		: Packet(14, true), player(_player) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(PlayerSpawnPacket)
	PACKET_TO_ENET(PlayerSpawnPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, player);
	}
};
