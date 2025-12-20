#pragma once
#include "Imports/common.h"
#include "Utils/NetUtils.h"
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <sstream>

/**
 * @brief Packet header structure
 *
 * This structure defines the header of a packet, which includes the type
 * and timestamp of the packet.
 */
struct PacketHeader {
	uint8_t type = 0; // Type of packet
	uint64_t timestamp = 0; // Timestamp of when the packet was sent

	template<class Archive>
	void serialize(Archive & archive) {
		archive(type, timestamp);
	}
};

/**
 * @brief Base Packet class
 *
 * Derived packets should:
 * 1. Call Packet(type) in their constructor
 * 2. Add their own data members
 * 3. Implement serialize() that archives all fields (header, is_reliable, then custom fields)
 * 4. Implement to_enet_packet() using the PACKET_TO_ENET macro
 */
class Packet {
public:
	PacketHeader header; // Header of the packet
	bool is_reliable = true; // Flag indicating if the packet is reliable (handled by ENet)

	Packet() = default;
	Packet(uint8_t type, bool reliable = true);
	virtual ~Packet() = default;

	// Converts this packet to an ENetPacket for sending
	virtual ENetPacket* to_enet_packet();

	// Default serialize for base packet
	template<class Archive>
	void serialize(Archive& archive) {
		archive(header, is_reliable);
	}
protected:
	// Helper template for derived classes to use in their to_enet_packet()
	template<typename Derived>
	ENetPacket* serialize_to_enet() {
		std::ostringstream os;
		cereal::BinaryOutputArchive archive(os);
		archive(static_cast<Derived&>(*this));
		std::string serialized = os.str();
		return enet_packet_create(
			serialized.c_str(),
			serialized.size(),
			is_reliable ? ENET_PACKET_FLAG_RELIABLE : 0
		);
	}
};

// Macro for easy to_enet_packet() implementation in derived classes
#define PACKET_TO_ENET(ClassName) \
	ENetPacket* to_enet_packet() override { \
		return serialize_to_enet<ClassName>(); \
	}

// Macro for easy deserialize() implementation in derived classes
#define PACKET_DESERIALIZE(ClassName) \
	static ClassName deserialize(const std::string& raw_data) { \
		ClassName packet; \
		std::istringstream is(raw_data); \
		cereal::BinaryInputArchive archive(is); \
		archive(packet); \
		return packet; \
	}