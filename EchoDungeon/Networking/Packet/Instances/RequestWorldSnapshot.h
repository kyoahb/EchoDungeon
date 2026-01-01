#pragma once
#include "Networking/Packet/Packet.h"
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>

// Packet type: 16
// Packet name: RequestWorldSnapshot

/**
 * @brief Client -> Server request for full world state snapshot.
 */
class RequestWorldSnapshotPacket : public Packet {
public:

	// Default constructor
	RequestWorldSnapshotPacket()
		: Packet(16, true) {
	}


	// Macros for serialization
	PACKET_DESERIALIZE(RequestWorldSnapshotPacket)
	PACKET_TO_ENET(RequestWorldSnapshotPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable);
	}
};
