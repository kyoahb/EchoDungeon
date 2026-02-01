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
#include "Networking/Packet/Instances/Player/PlayerSpawn.h"
#include "Networking/Packet/Instances/Player/PlayerUpdate.h"
#include "Networking/Packet/Instances/Player/PlayerDestroy.h"
#include "Networking/Packet/Instances/Enemy/EnemySpawn.h"
#include "Networking/Packet/Instances/Enemy/EnemyUpdate.h"
#include "Networking/Packet/Instances/Enemy/EnemyDestroy.h"
#include "Networking/Packet/Instances/Object/ObjectSpawn.h"
#include "Networking/Packet/Instances/Object/ObjectDestroy.h"
#include "Networking/Packet/Instances/PlayerInput.h"
#include "Networking/Packet/Instances/RequestWorldSnapshot.h"
#include "Networking/Packet/Instances/Player/PlayerAttack.h"

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

	// PlayerInput Event (Packet ID: 9)
	SERVER_PACKET_EVENT_DECLARATION(PlayerInput)

    // RequestWorldSnapshot Event (Packet ID: 10)
    SERVER_PACKET_EVENT_DECLARATION(RequestWorldSnapshot)

    // PlayerAttack Event (Packet ID: 22)
    SERVER_PACKET_EVENT_DECLARATION(PlayerAttack)

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

	// WorldSnapshot Event (Packet ID: 11)
	CLIENT_PACKET_EVENT_DECLARATION(WorldSnapshot)

	// PlayerSpawn Event (Packet ID: 14)
	CLIENT_PACKET_EVENT_DECLARATION(PlayerSpawn)

	// PlayerUpdate Event (Packet ID: 15)
	CLIENT_PACKET_EVENT_DECLARATION(PlayerUpdate)

	// PlayerDestroy Event (Packet ID: 16)
	CLIENT_PACKET_EVENT_DECLARATION(PlayerDestroy)

	// EnemySpawn Event (Packet ID: 17)
	CLIENT_PACKET_EVENT_DECLARATION(EnemySpawn)

	// EnemyUpdate Event (Packet ID: 18)
	CLIENT_PACKET_EVENT_DECLARATION(EnemyUpdate)

	// EnemyDestroy Event (Packet ID: 19)
	CLIENT_PACKET_EVENT_DECLARATION(EnemyDestroy)

	// ObjectSpawn Event (Packet ID: 20)
	CLIENT_PACKET_EVENT_DECLARATION(ObjectSpawn)

	// ObjectDestroy Event (Packet ID: 21)
	CLIENT_PACKET_EVENT_DECLARATION(ObjectDestroy)


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
