#include "Packet.h"

/**
 * @brief Constructs a Packet with a specific type.
 * @param type The packet type ID.
 * @param reliable Whether the packet should be sent reliably.
 */
Packet::Packet(uint8_t type, bool reliable) 
	: is_reliable(reliable) {
	header.type = type;
	header.timestamp = 0;
}

/**
 * @brief Converts the base packet to an ENetPacket.
 * Derived classes should override this using PACKET_TO_ENET macro.
 * @return A pointer to the created ENetPacket.
 */
ENetPacket* Packet::to_enet_packet() {
	return serialize_to_enet<Packet>();
}