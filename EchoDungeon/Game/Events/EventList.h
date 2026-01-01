#pragma once
#include "Game/Events/Event.h"
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

#define SERVER_PACKET_EVENT_DECLARATION(BaseName) \
    class BaseName##EventData : public BaseEventData { \
    public: \
        BaseName##Packet packet; \
        ENetPeer* peer; \
        BaseName##EventData(const BaseName##Packet& p, ENetPeer* pr) \
            : packet(p), peer(pr) {} \
    }; \
    using BaseName##Event = Event<BaseName##EventData>;

#define CLIENT_PACKET_EVENT_DECLARATION(BaseName) \
    class BaseName##EventData : public BaseEventData { \
    public: \
        BaseName##Packet packet; \
        BaseName##EventData(const BaseName##Packet& p) : packet(p) {} \
    }; \
    using BaseName##Event = Event<BaseName##EventData>;


// ============================================================================
// SERVER-SIDE PACKET EVENTS
// Events triggered when the server receives specific packets from clients
// ============================================================================
namespace ServerEvents {

	// ConnectionInitiation Event (Packet ID: 1)
    SERVER_PACKET_EVENT_DECLARATION(ConnectionInitiation)

	// DisconnectionInfo Event (Packet ID: 4)
    SERVER_PACKET_EVENT_DECLARATION(DisconnectInfo)

	// GeneralInformationUpdate Event (Packet ID: 8)
	SERVER_PACKET_EVENT_DECLARATION(GeneralInformationUpdate)

	// PlayerInput Event (Packet ID: 15)
	SERVER_PACKET_EVENT_DECLARATION(PlayerInput)

    // RequestWorldSnapshot Event (Packet ID: 16)
    SERVER_PACKET_EVENT_DECLARATION(RequestWorldSnapshot)

    // Pure events

	// ENET_EVENT_TYPE_CONNECT Event
	class ConnectionEventData : public BaseEventData {
    public:
        const ENetEvent& event;
        ConnectionEventData(const ENetEvent& _event)
            : event(_event) {}
	};
    using ConnectionEvent = Event<ConnectionEventData>;

	// ENET_EVENT_TYPE_DISCONNECT Event
    class DisconnectEventData : public BaseEventData {
    public:
        const ENetEvent& event;
        DisconnectEventData(const ENetEvent& _event)
            : event(_event) {
        }
    };
    using DisconnectEvent = Event<DisconnectEventData>;

	// ENET_EVENT_TYPE_DISCONNECT_TIMEOUT Event
    class DisconnectTimeoutEventData : public BaseEventData {
    public:
        const ENetEvent& event;
        DisconnectTimeoutEventData(const ENetEvent& _event)
            : event(_event) {
        }
    };
    using DisconnectTimeoutEvent = Event<DisconnectTimeoutEventData>;

}

// ============================================================================
// CLIENT-SIDE PACKET EVENTS
// Events triggered when the client receives specific packets from the server
// ============================================================================
namespace ClientEvents {

	// ConnectionRefusal Event (Packet ID: 2)
	CLIENT_PACKET_EVENT_DECLARATION(ConnectionRefusal)

    // ConnectionConfirmation Event (Packet ID: 3)
	CLIENT_PACKET_EVENT_DECLARATION(ConnectionConfirmation)

	// DisconnectKick Event (Packet ID: 5)
	CLIENT_PACKET_EVENT_DECLARATION(DisconnectKick)

	// UserDataUpdate Event (Packet ID: 6)
	CLIENT_PACKET_EVENT_DECLARATION(ServerDataUpdate)

	// StateChange Event (Packet ID: 7)
	CLIENT_PACKET_EVENT_DECLARATION(StateChange)

	// WorldSnapshot Event (Packet ID: 10)
	CLIENT_PACKET_EVENT_DECLARATION(WorldSnapshot)

	// EntityUpdate Event (Packet ID: 11)
	CLIENT_PACKET_EVENT_DECLARATION(EntityUpdate)

	// EntitySpawn Event (Packet ID: 12)
	CLIENT_PACKET_EVENT_DECLARATION(EntitySpawn)

	// EntityDestroy Event (Packet ID: 13)
	CLIENT_PACKET_EVENT_DECLARATION(EntityDestroy)

	// PlayerSpawn Event (Packet ID: 14)
	CLIENT_PACKET_EVENT_DECLARATION(PlayerSpawn)


    // Pure events

    // ENET_EVENT_TYPE_CONNECT Event
    class ConnectionEventData : public BaseEventData {
    public:
        const ENetEvent& event;
        ConnectionEventData(const ENetEvent& _event)
            : event(_event) {
        }
    };
    using ConnectionEvent = Event<ConnectionEventData>;

    // ENET_EVENT_TYPE_DISCONNECT Event
    class DisconnectEventData : public BaseEventData {
    public:
        const ENetEvent& event;
        DisconnectEventData(const ENetEvent& _event)
            : event(_event) {
        }
    };
    using DisconnectEvent = Event<DisconnectEventData>;

    // State change Event (called after this client changes state)
	class PostStateChangeEventData : public BaseEventData {
    public:
        std::string new_state;
        PostStateChangeEventData(const std::string& _new_state)
            : new_state(_new_state) {}
    };
	using PostStateChangeEvent = Event<PostStateChangeEventData>;
}
