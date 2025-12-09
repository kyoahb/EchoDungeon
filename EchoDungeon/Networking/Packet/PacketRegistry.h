#pragma once

#include "Packet.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

class PacketRegistry {
public:
    static void registerPacket(uint8_t id, const std::string& name, std::function<std::unique_ptr<Packet>(Packet&&)> converter);
    static std::unique_ptr<Packet> deserialize(ENetPacket* packet);
    static std::string getPacketName(uint8_t id);
private:
    static std::unordered_map<uint8_t, std::pair<std::string, std::function<std::unique_ptr<Packet>(Packet&&)>>> registry;
};