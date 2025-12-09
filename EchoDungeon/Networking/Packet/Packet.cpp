#include "Packet.h"

/**
 * @brief Constructs a Packet from an ENetPacket.
 * @param packet The ENetPacket to construct from.
 */
Packet::Packet(ENetPacket* packet) {
	if (packet) {
		data.assign((char*)packet->data, packet->dataLength); // Copy binary data to the string
		header.type = 0; // Type will be set during deserialization
		header.size = packet->dataLength; // Size of the data
		header.timestamp = 0; // Timestamp can be set later if needed
		is_reliable = (packet->flags & ENET_PACKET_FLAG_RELIABLE) != 0; // Check reliability flag
	}
}

Packet::Packet(const PacketHeader& _header, const char* _data, bool _is_reliable) : 
	header(_header), data(_data ? std::string(_data, _header.size) : ""), is_reliable(_is_reliable) {}

Packet::Packet(uint8_t type, const char* _data, size_t size, bool _is_reliable) : 
	header{ type, (uint32_t)size, 0 }, data(_data ? std::string(_data, size) : ""), is_reliable(_is_reliable) {}

/**
 * @brief Converts the packet to an ENetPacket.
 * @return A pointer to the created ENetPacket.
 */
ENetPacket* Packet::to_enet_packet() const {
	// Update size in case data changed
	header.size = data.size();
	ENetPacket* enet_packet = enet_packet_create(data.c_str(), data.size(), is_reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
	return enet_packet;
}

/**
 * @brief Checks if an ENetEvent is a packet receive event.
 * @param event The ENetEvent to check.
 * @return True if the event is a receive event, false otherwise.
 */
bool Packet::is_event_packet(const ENetEvent event) {
	return event.type == ENET_EVENT_TYPE_RECEIVE;
}