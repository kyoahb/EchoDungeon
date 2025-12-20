#include "Client.h"
#include "Networking/Packet/PacketRegistry.h"
#include "Networking/Packet/Instances/ConnectionInitiation.h"
#include "Networking/Packet/Instances/ConnectionConfirmation.h"
#include "Networking/Packet/Instances/ConnectionRefusal.h"
#include "Networking/Packet/Instances/ServerDataUpdate.h"
#include "Game/Events/EventList.h"

Client::Client(const std::string& username) : NetworkUser(), peers(ClientPeerlist()) {
	this->username = username;
	connection_state = ClientConnectionState::DISCONNECTED;

	// Create client host. No address + 1 max peer since this is a client
	host = enet_host_create(nullptr, 1, NetworkConstants::MAX_CHANNELS, 
		NetworkConstants::BANDWIDTH_LIMIT, NetworkConstants::BANDWIDTH_LIMIT);

	if (host == nullptr) {
		ERROR("Failed to create ENet host");
		return;
	}

	// Register event callbacks for connection flow packets
	
	ClientEvents::ConnectionConfirmationEvent::register_callback(
		[this](const ClientEvents::ConnectionConfirmationEventData& data) {
			handle_connection_confirmation(data.packet.client_assigned_id);
		}
	);
	
	ClientEvents::ConnectionRefusalEvent::register_callback(
		[this](const ClientEvents::ConnectionRefusalEventData& data) {
			handle_connection_refusal(data.packet.refusal_reason);
		}
	);
	
	ClientEvents::ServerDataUpdateEvent::register_callback(
		[this](const ClientEvents::ServerDataUpdateEventData& data) {
			handle_server_data_update(data.packet.current_peers, data.packet.server_info);
		}
	);
}

Client::~Client() {
	// Disconnect patiently
	if (is_connected()) {
		disconnect().get();
	}

	// Stop networking loop if running
	if (is_running.load()) {
		stop();
	}
	// Destroy ENet host
	if (host) {
		enet_host_destroy(host);
		host = nullptr;
	}
}

/*
* @brief Checks if the client is fully connected to a server.
* @return true if fully connected, false otherwise.
*/
bool Client::is_connected() const {
	return connection_state == ClientConnectionState::CONNECTED;
}

/*
* @brief Sends a packet to the connected server.
* @param packet The packet to send.
*/
bool Client::send_packet(Packet& packet) {
	TRACE("Sending packet " + PacketRegistry::getPacketName(packet.header.type) + " to server");

	if (peers.server_peer == nullptr || peers.server_peer->state != ENET_PEER_STATE_CONNECTED) {
		ERROR("Client is not connected, cannot send packet");
		return false;
	}

	return NetworkUser::send_packet(packet.to_enet_packet(), peers.server_peer);
}


// ============================================================================
// UPDATE LOOP
// ============================================================================

/*
* @brief Begins the client's networking update loop.
*/
void Client::start() {
	if (is_running) {
		WARNING("Client update loop is already running, cannot start again");
		return;
	}

	if (thread_id == std::this_thread::get_id()) {
		WARNING("Client update loop already exists in this thread, cannot start");
		return;
	}

	NetworkUser::start();
}

/*
* @brief Stops the client's networking update loop.
*/
void Client::stop() {
	if (!is_running) {
		WARNING("Client update loop is not running, cannot stop");
		return;
	}

	NetworkUser::stop();
}

/*
* @brief Executes a single update cycle for the client, processing incoming events and packets.
*/
void Client::update() {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT: {
				INFO("ENet connection established, sending ConnectionInitiation packet");
				connection_state = ClientConnectionState::AWAITING_CONFIRMATION;
				
				// Send ConnectionInitiation packet with our username
				auto init_packet = ConnectionInitiationPacket(username);
				send_packet(init_packet);
				
				// Trigger event for external listeners
				ClientEvents::ConnectionEventData data(event);
				ClientEvents::ConnectionEvent::trigger(data);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE: {
				// Deserialize and trigger packet event
				PacketRegistry::handleClientPacket(event.packet);
				enet_packet_destroy(event.packet);
				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT: {
				INFO("Disconnected from server");
				
				// If we were in the middle of connecting, fail the connection
				if (connection_promise.has_value() && connection_state != ClientConnectionState::CONNECTED) {
					connection_promise->set_value({false, "Disconnected during connection handshake"});
					connection_promise.reset();
				}
				
				peers.server_peer = nullptr;
				connection_state = ClientConnectionState::DISCONNECTED;
				
				// Trigger event for external listeners
				ClientEvents::DisconnectEventData data(event);
				ClientEvents::DisconnectEvent::trigger(data);
				break;
			}
			default:
				break;
		}
	}
}

// ============================================================================
// CONNECTION FLOW HANDLERS
// ============================================================================

/*
* @brief Handles receiving a ConnectionConfirmation packet from the server.
* @param assigned_id The server-side ID assigned to this client.
*/
void Client::handle_connection_confirmation(uint16_t assigned_id) {
	if (connection_state != ClientConnectionState::AWAITING_CONFIRMATION) {
		WARNING("Received ConnectionConfirmation but not in AWAITING_CONFIRMATION state");
		return;
	}
	
	INFO("Connection confirmed! Assigned ID: " + std::to_string(assigned_id));
	connection_state = ClientConnectionState::AWAITING_SERVER_DATA;
	peers.set_local_server_side_id(assigned_id);
}

/*
* @brief Handles receiving a ConnectionRefusal packet from the server.
* @param reason The reason for the connection refusal.
*/
void Client::handle_connection_refusal(const std::string& reason) {
	WARNING("Connection refused by server: " + reason);
	
	connection_state = ClientConnectionState::DISCONNECTED;
	peers.server_peer = nullptr;
	
	// Fulfill the connection promise with failure
	if (connection_promise.has_value()) {
		connection_promise->set_value({false, reason});
		connection_promise.reset();
	}
}

/*
* @brief Handles receiving a ServerDataUpdate packet from the server.
* @param peers_data Map of all connected peers' data.
* @param server_info Information about the server.
*/
void Client::handle_server_data_update(const std::unordered_map<uint16_t, UserData>& peers_data, const OpenServer& server_info) {
	INFO("Received server data update with " + std::to_string(peers_data.size()) + " peers");
	
	// Store server information
	connected_server_info = server_info;
	
	// Populate the peerlist
	peers.clear();
	for (const auto& [id, user_data] : peers_data) {
		peers.add_peer(user_data);
	}
	
	// If we were waiting for this to complete connection, mark as connected
	if (connection_state == ClientConnectionState::AWAITING_SERVER_DATA) {
		connection_state = ClientConnectionState::CONNECTED;
		INFO("Connection flow complete - now fully connected!");
		
		// Fulfill the connection promise with success
		if (connection_promise.has_value()) {
			connection_promise->set_value({true, ""});
			connection_promise.reset();
		}
	}
}

// ============================================================================
// CONNECTION METHODS
// ============================================================================

/*
* @brief Connects the client to a server at the specified IP and port.
* @param ip The IP address of the server.
* @param port The port of the server.
* @return A future containing the connection result.
*/
std::future<ConnectionResult> Client::connect(const std::string& ip, uint16_t port) {
	// Check if already connecting or connected
	if (connection_state != ClientConnectionState::DISCONNECTED) {
		WARNING("Cannot connect: already connected or connecting");
		std::promise<ConnectionResult> immediate_promise;
		immediate_promise.set_value({false, "Already connected or connecting"});
		return immediate_promise.get_future();
	}
	
	// Setup ENetAddress
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	// Initiate ENet connection
	ENetPeer* server_peer = enet_host_connect(host, &address, NetworkConstants::MAX_CHANNELS, 0);
	if (server_peer == nullptr) {
		ERROR("Failed to initiate connection to server at " + ip + ":" + std::to_string(port));
		std::promise<ConnectionResult> immediate_promise;
		immediate_promise.set_value({false, "Failed to initiate ENet connection"});
		return immediate_promise.get_future();
	}

	// Store the server peer
	peers.server_peer = server_peer;
	connection_state = ClientConnectionState::CONNECTING;
	
	INFO("Initiated connection to server at " + ip + ":" + std::to_string(port));

	// Create promise for async result - will be fulfilled by handlers
	connection_promise.emplace();
	
	// Start the network loop if not already running
	if (!is_running) {
		start();
	}

	// Return future that will be fulfilled when connection flow completes
	return connection_promise->get_future();
}

/*
* @brief Patiently disconnects the client from the server, waiting for confirmation.
* @return true if the disconnection was successful, false otherwise.
*/
std::future<bool> Client::disconnect() {
	// FOR NOW, call force_disconnect
	bool status = force_disconnect();

	INFO("Successfully disconnected from server");
	return std::async(std::launch::deferred, [status] { return status; });
}

/*
* @brief Forcefully disconnects the client from the server without waiting for confirmation.
* @return true if the disconnection was successful, false otherwise.
*/
bool Client::force_disconnect() {
	if (peers.server_peer == nullptr) {
		WARNING("Client is not connected, cannot force disconnect");
		return false;
	}

	enet_peer_disconnect_now(peers.server_peer, 0);
	peers.server_peer = nullptr;
	peers.clear();
	connection_state = ClientConnectionState::DISCONNECTED;
	
	// If there's a pending connection promise, fail it
	if (connection_promise.has_value()) {
		connection_promise->set_value({false, "Force disconnected"});
		connection_promise.reset();
	}
	
	INFO("Client forcefully disconnected from server");
	return true;
}
