#pragma once
#include "Imports/common.h"
#include "Utils/NetUtils.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

/**
 * @brief Packet header structure
 *
 * This structure defines the header of a packet, which includes the type, direction,
 * subtype, size, and timestamp of the packet.
 */
struct PacketHeader {
	uint8_t type = 0; // Type of packet
	uint32_t size = 0; // Size of the packet data
	uint64_t timestamp = 0; // Timestamp of when the packet was sent

	template<class Archive>
	void serialize(Archive & archive) {
		archive(type, size, timestamp);
	}
};

/**
 * @brief Packet structure
 *
 * This structure defines a packet, which includes the header and the data of the packet.
 */
class Packet {
public:
	mutable PacketHeader header; // Header of the packet
	std::string data; // Data of the packet
	bool is_reliable = true; // Flag indicating if the packet is reliable (handled by ENet)

	Packet() = default;
	Packet(ENetPacket* packet);
	Packet(const PacketHeader& _header, const char* _data, bool _is_reliable = true);
	Packet(uint8_t type, const char* _data, size_t size, bool _is_reliable = true);
	virtual ~Packet() = default;

	ENetPacket* to_enet_packet() const;

	static bool is_event_packet(const ENetEvent event);

	template<class Archive>
	void serialize(Archive & archive) {
		archive(header, data, is_reliable);
	}
};