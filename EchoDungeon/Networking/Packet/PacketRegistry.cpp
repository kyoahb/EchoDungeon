#include "PacketRegistry.h"
#include <cereal/archives/binary.hpp>
#include <sstream>

std::unordered_map<uint8_t, std::pair<std::string, std::function<std::unique_ptr<Packet>(Packet&&)>>> PacketRegistry::registry;

/**
 * @brief Registers a packet type with its ID, name, and converter function.
 * @param id The unique packet type ID.
 * @param name The human-readable name of the packet.
 * @param converter The function to convert a base Packet to the specific derived type.
 */
void PacketRegistry::registerPacket(uint8_t id, const std::string& name, std::function<std::unique_ptr<Packet>(Packet&&)> converter) {
    registry[id] = {name, converter};
}

/**
 * @brief Converts an ENetPacket to a specific Packet derived type.
 * @param packet The ENetPacket to convert.
 * @returns A unique pointer to the specific Packet derived type, or nullptr if conversion fails.
 */
std::unique_ptr<Packet> PacketRegistry::deserialize(ENetPacket* packet) {
    if (!packet || packet->dataLength == 0) return nullptr; // Invalid packet

	// Convert binary to data using the cereal library
    std::istringstream is(std::string((char*)packet->data, packet->dataLength));
    cereal::BinaryInputArchive archive(is);
    Packet base;
    archive(base);

    auto it = registry.find(base.header.type); // Locate packet converter
	if (it == registry.end()) return nullptr; // Unknown packet type

	return it->second.second(std::move(base)); // Convert to specific packet type
}

/**
 * @brief Fetches the human-readable name of a packet type by its ID.
 * @param id The unique packet type ID.
 * @returns The name of the packet type, or "Unknown" if not registered.
 */
std::string PacketRegistry::getPacketName(uint8_t id) {
    auto it = registry.find(id);
    if (it == registry.end()) return "Unknown";
    return it->second.first;
}