#pragma once
#include "Networking/Packet/Packet.h"

// Packet type: 24
// Packet name: ItemDiscard
// Direction: Client -> Server
// Purpose: Request to discard an item from inventory

class ItemDiscardPacket : public Packet {
public:
	uint32_t item_id = 0;  // ID of item to discard

	// Default constructor
	ItemDiscardPacket()
		: Packet(24, true) {
	}

	// Constructor with data
	ItemDiscardPacket(uint32_t _item_id)
		: Packet(24, true), item_id(_item_id) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ItemDiscardPacket)
	PACKET_TO_ENET(ItemDiscardPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, item_id);
	}
};
