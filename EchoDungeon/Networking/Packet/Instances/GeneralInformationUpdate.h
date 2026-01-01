#pragma once
#include "Networking/Packet/Packet.h"
#include "Networking/User/UserData.h"
#include <cereal/types/unordered_map.hpp>
#include "Networking/Server/OpenServer.h"

// Packet type: 8
// Packet name: GeneralInformationUpdate

/**
 * @brief Packet sent by client to server to reveal important general information.
 */
class GeneralInformationUpdatePacket : public Packet {
public:
	std::string current_state; // Name of the current state the client is on

	// Default constructor
	GeneralInformationUpdatePacket()
		: Packet(8, true), current_state("") {
	}

	// Constructor with data
	GeneralInformationUpdatePacket(const std::string& _current_state)
		: Packet(8, true), current_state(_current_state) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(GeneralInformationUpdatePacket)
		PACKET_TO_ENET(GeneralInformationUpdatePacket)

		template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, current_state);
	}
};
