#include "PacketRegistry.h"
#include "Networking/Packet/Instances/ConnectionInitiation.h"
#include "Networking/Packet/Instances/ConnectionRefusal.h"
#include "Networking/Packet/Instances/ConnectionConfirmation.h"
#include "Networking/Packet/Instances/DisconnectInfo.h"
#include "Networking/Packet/Instances/DisconnectKick.h"
#include "Networking/Packet/Instances/ServerDataUpdate.h"
#include "Networking/Packet/Instances/GeneralInformationUpdate.h"
#include "Networking/Packet/Instances/StateChange.h"
#include "Networking/Packet/Instances/WorldSnapshot.h"
#include "Networking/Packet/Instances/EntityUpdate.h"
#include "Networking/Packet/Instances/EntitySpawn.h"
#include "Networking/Packet/Instances/EntityDestroy.h"
#include "Networking/Packet/Instances/PlayerSpawn.h"
#include "Networking/Packet/Instances/PlayerInput.h"
#include "Networking/Packet/Instances/RequestWorldSnapshot.h"

#include "Game/Events/EventList.h"

#define CLIENT_SIDE_EVENT_HANDLER(BaseName) \
    [](Packet& packet) { \
        auto& typed_packet = static_cast<BaseName##Packet&>(packet); \
        ClientEvents::BaseName##EventData data(typed_packet); \
        ClientEvents::BaseName##Event::trigger(data); \
    }

#define SERVER_SIDE_EVENT_HANDLER(BaseName) \
    [](Packet& packet, ENetPeer* peer) { \
        auto& typed_packet = static_cast<BaseName##Packet&>(packet); \
        ServerEvents::BaseName##EventData data(typed_packet, peer); \
        ServerEvents::BaseName##Event::trigger(data); \
    }

#define PACKET_CONVERTER(BaseName) \
    [](const std::string& raw_data) { \
        return std::make_unique<BaseName##Packet>(BaseName##Packet::deserialize(raw_data)); \
    }

// Macro for server-only packets (Client -> Server)
#define REGISTER_SERVER_PACKET(Type, BaseName) \
    registerPacket(Type, #BaseName, PACKET_CONVERTER(BaseName), SERVER_SIDE_EVENT_HANDLER(BaseName), nullptr)

// Macro for client-only packets (Server -> Client)
#define REGISTER_CLIENT_PACKET(Type, BaseName) \
    registerPacket(Type, #BaseName, PACKET_CONVERTER(BaseName), nullptr, CLIENT_SIDE_EVENT_HANDLER(BaseName))

// Macro for bidirectional packets (both directions)
#define REGISTER_BIDIRECTIONAL_PACKET(Type, BaseName) \
    registerPacket(Type, #BaseName, PACKET_CONVERTER(BaseName), SERVER_SIDE_EVENT_HANDLER(BaseName), CLIENT_SIDE_EVENT_HANDLER(BaseName))

/**
 * @brief Initializes all packet types in the registry.
 * Call this once at application startup before any networking operations.
 */
void PacketRegistry::initializeRegistry() {
    // Client -> Server packets
    REGISTER_SERVER_PACKET(1, ConnectionInitiation);
    REGISTER_SERVER_PACKET(4, DisconnectInfo);
	REGISTER_SERVER_PACKET(8, GeneralInformationUpdate);
	REGISTER_SERVER_PACKET(15, PlayerInput);
    REGISTER_SERVER_PACKET(16, RequestWorldSnapshot);

    // Server -> Client packets
    REGISTER_CLIENT_PACKET(2, ConnectionRefusal);
    REGISTER_CLIENT_PACKET(3, ConnectionConfirmation);
    REGISTER_CLIENT_PACKET(5, DisconnectKick);
    REGISTER_CLIENT_PACKET(6, ServerDataUpdate);
	REGISTER_CLIENT_PACKET(7, StateChange);
	REGISTER_CLIENT_PACKET(10, WorldSnapshot);
	REGISTER_CLIENT_PACKET(11, EntityUpdate);
	REGISTER_CLIENT_PACKET(12, EntitySpawn);
	REGISTER_CLIENT_PACKET(13, EntityDestroy);
	REGISTER_CLIENT_PACKET(14, PlayerSpawn);
}
