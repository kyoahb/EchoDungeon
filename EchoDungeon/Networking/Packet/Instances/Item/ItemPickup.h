#pragma once
#include "Networking/Packet/Packet.h"
#include "Game/World/Entities/Item.h"

// Packet type: 23
// Packet name: ItemPickup
// Direction: Server -> Client
// Purpose: Notify client that a player received an item

class ItemPickupPacket : public Packet {
public:
	uint32_t player_id = 0;  // Which player received the item
	Item item;  // Full item data

	// Default constructor
	ItemPickupPacket()
		: Packet(23, true) {
	}

	// Constructor with data
	ItemPickupPacket(uint32_t _player_id, const Item& _item)
		: Packet(23, true), player_id(_player_id), item(_item) {
	}

	// Macros for serialization
	PACKET_DESERIALIZE(ItemPickupPacket)
	PACKET_TO_ENET(ItemPickupPacket)

	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable, player_id, item);
	}
};
