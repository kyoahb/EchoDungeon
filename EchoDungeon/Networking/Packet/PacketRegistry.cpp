#include "PacketRegistry.h"
#include <cereal/archives/binary.hpp>
#include <sstream>

std::unordered_map<uint8_t, PacketEntry> PacketRegistry::registry;

/**
 * @brief Registers a packet type with its ID, name, converter, and event triggers.
 * @param id The unique packet type ID.
 * @param name The human-readable name of the packet.
 * @param converter The function to convert raw data to the specific derived type.
 * @param server_trigger The function to trigger server-side event for this packet.
 * @param client_trigger The function to trigger client-side event for this packet.
 */
void PacketRegistry::registerPacket(
    uint8_t id, 
    const std::string& name, 
    std::function<std::unique_ptr<Packet>(const std::string&)> converter,
    ServerEventTrigger server_trigger,
    ClientEventTrigger client_trigger
) {
    registry[id] = {name, converter, server_trigger, client_trigger};
}

/**
 * @brief Converts an ENetPacket to a specific Packet derived type.
 * @param packet The ENetPacket to convert.
 * @returns A unique pointer to the specific Packet derived type, or nullptr if conversion fails.
 */
std::unique_ptr<Packet> PacketRegistry::deserialize(ENetPacket* packet) {
    if (!packet || packet->dataLength == 0) return nullptr;

    // Get raw data as string
    std::string raw_data((char*)packet->data, packet->dataLength);
    
    // Check packet type by looking at header
    std::istringstream is(raw_data);
    cereal::BinaryInputArchive archive(is);
    PacketHeader header;
    archive(header);

    // Find the converter for this packet type
    auto it = registry.find(header.type);
    if (it == registry.end()) return nullptr;

    // Use the converter to create the specific packet type
    return it->second.converter(raw_data);
}

/**
 * @brief Deserializes a packet and triggers the server-side event.
 * @param peer The peer that sent the packet.
 * @param enet_packet The raw ENetPacket.
 */
void PacketRegistry::handleServerPacket(ENetPeer* peer, ENetPacket* enet_packet) {
    auto packet = deserialize(enet_packet);
    if (!packet) {
        WARNING("Failed to deserialize packet");
        return;
    }

    auto it = registry.find(packet->header.type);
    if (it == registry.end() || !it->second.server_event_trigger) {
        WARNING("No server event trigger for packet type: " + std::to_string(packet->header.type));
        return;
    }

    TRACE("Triggering server event for packet: " + it->second.name);
    it->second.server_event_trigger(*packet, peer);
}

/**
 * @brief Deserializes a packet and triggers the client-side event.
 * @param enet_packet The raw ENetPacket.
 */
void PacketRegistry::handleClientPacket(ENetPacket* enet_packet) {
    auto packet = deserialize(enet_packet);
    if (!packet) {
        WARNING("Failed to deserialize packet");
        return;
    }

    auto it = registry.find(packet->header.type);
    if (it == registry.end() || !it->second.client_event_trigger) {
        WARNING("No client event trigger for packet type: " + std::to_string(packet->header.type));
        return;
    }

    TRACE("Triggering client event for packet: " + it->second.name);
    it->second.client_event_trigger(*packet);
}

/**
 * @brief Fetches the human-readable name of a packet type by its ID.
 * @param id The unique packet type ID.
 * @returns The name of the packet type, or "Unknown" if not registered.
 */
std::string PacketRegistry::getPacketName(uint8_t id) {
    auto it = registry.find(id);
    if (it == registry.end()) return "Unknown";
    return it->second.name;
}