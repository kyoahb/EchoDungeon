#pragma once
#include "Networking/Packet/Packet.h"
#include "Networking/User/UserData.h"
#include <cereal/types/unordered_map.hpp>
#include "Networking/Server/OpenServer.h"

// Packet type: 7
// Packet name: StateChange

/**
 * @brief Packet sent by server to get clients to change to the same state.
 */
class StateChangePacket : public Packet {
public:
	std::string new_state; // Name of the new state to change to

	// Default constructor
	StateChangePacket()
		: Packet(7, true), new_state("") {
	}

	// Constructor with data
	StateChangePacket(const std::string& _new_state)
		: Packet(7, true), new_state(_new_state) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(StateChangePacket)
		PACKET_TO_ENET(StateChangePacket)

		template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, new_state);
	}
};
