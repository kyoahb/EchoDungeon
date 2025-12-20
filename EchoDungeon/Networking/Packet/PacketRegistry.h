#pragma once

#include "Packet.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

// Trigger function types - these trigger events for the appropriate packet type
using ServerEventTrigger = std::function<void(Packet&, ENetPeer*)>;
using ClientEventTrigger = std::function<void(Packet&)>;

struct PacketEntry {
    std::string name;
    std::function<std::unique_ptr<Packet>(const std::string&)> converter;
    ServerEventTrigger server_event_trigger;
    ClientEventTrigger client_event_trigger;
};

class PacketRegistry {
public:
    // Register a packet type with event triggers
    static void registerPacket(
        uint8_t id, 
        const std::string& name, 
        std::function<std::unique_ptr<Packet>(const std::string&)> converter,
        ServerEventTrigger server_trigger = nullptr,
        ClientEventTrigger client_trigger = nullptr
    );
    
    // Initialize all packet types (call once at startup)
    static void initializeRegistry();
    
    // Deserialize and trigger server event for packet
    static void handleServerPacket(ENetPeer* peer, ENetPacket* enet_packet);
    
    // Deserialize and trigger client event for packet
    static void handleClientPacket(ENetPacket* enet_packet);
    
    // Deserialize an ENetPacket to the appropriate derived packet type
    static std::unique_ptr<Packet> deserialize(ENetPacket* packet);
    
    // Get the human-readable name of a packet type
    static std::string getPacketName(uint8_t id);

private:
    static std::unordered_map<uint8_t, PacketEntry> registry;
};