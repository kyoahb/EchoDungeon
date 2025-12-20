#pragma once
#include "Game/Events/Event.h"
#include "Networking/Packet/Instances/ConnectionInitiation.h"
#include "Networking/Packet/Instances/ConnectionRefusal.h"
#include "Networking/Packet/Instances/ConnectionConfirmation.h"
#include "Networking/Packet/Instances/DisconnectInfo.h"
#include "Networking/Packet/Instances/DisconnectKick.h"
#include "Networking/Packet/Instances/ServerDataUpdate.h"


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
}
