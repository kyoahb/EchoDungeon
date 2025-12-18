#include "Client.h"
#include "Networking/Packet/PacketRegistry.h"
Client::Client() : NetworkUser(), peers(ClientPeerlist()) {

	// Create client host. No address + 1 max peer since this is a client
	host = enet_host_create(nullptr, 1, NetworkConstants::MAX_CHANNELS, 
		NetworkConstants::BANDWIDTH_LIMIT, NetworkConstants::BANDWIDTH_LIMIT);

	if (host == nullptr) {
		ERROR("Failed to create ENet host");
		return;
	}
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
* @brief Checks if the client is currently connected to a server.
* @return true if the currently connected to a server, false otherwise.
*/
bool Client::is_connected() const {
	return peers.server_peer != nullptr && peers.server_peer->state == ENET_PEER_STATE_CONNECTED;
}

/*
* @brief Sends a packet to the connected server.
* @param packet The packet to send.
*/
bool Client::send_packet(const Packet& packet) {
	TRACE("Sending packet " + PacketRegistry::getPacketName(packet.header.type) + " to server");

	if (!is_connected()) {
		ERROR("Client is not connected, cannot send packet");
		return false;
	}

	return NetworkUser::send_packet(packet.to_enet_packet(), peers.server_peer);
}


// UPDATE LOOP

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

	// Trigger client networking loop start event
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

	// Trigger client networking loop stop event
}

/*
* @brief Executes a single update cycle for the client, processing incoming events and packets.
*/
void Client::update() {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		// Identify packets, send off events
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				INFO("Successfully connected to server");
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				// Handle packets
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				INFO("Disconnected from server");
				peers.server_peer = nullptr;
				break;
			default:
				break;
		}
		// Implement a client connection timeout before failing
	}
}

/*
* @brief Connects the client to a server at the specified IP and port.
* @return true if the connection was successful, false otherwise.
*/
std::future<bool> Client::connect(const std::string& ip, uint16_t port) {
	// TO BE IMPLEMENTED

	// FOR NOW,
	// Setup ENetAddress* (instantiated in NetworkUser)
	enet_address_set_host(&address, ip.c_str());
	address.port = port;

	// Built-in ENet connect functionality.
	ENetPeer* server_peer = enet_host_connect(host, &address, 2, 0);
	if (server_peer == nullptr) {
		ERROR("Failed to initiate connection to server at " + ip + ":" + std::to_string(port));
		return std::async(std::launch::deferred, [] { return false; });
	}

	peers.server_peer = server_peer; // Attach server peer to peerlist

	INFO("Successfully initiated connection to server at " + ip + ":" + std::to_string(port));
	// Return a dummy future<bool>
	return std::async(std::launch::deferred, [] { return true; });
}

/*
* @brief Patiently disconnects the client from the server, waiting for confirmation.
* @return true if the disconnection was successful, false otherwise.
*/
std::future<bool> Client::disconnect() {
	// TO BE IMPLEMENTED

	// FOR NOW,
	// Call force_disconnect
	bool status = force_disconnect();

	INFO("Successfully disconnected from server");
	// Return a dummy future<bool>
	return std::async(std::launch::deferred, [status] { return status; });
}

/*
* @brief Forcefully disconnects the client from the server without waiting for confirmation.
* @return true if the disconnection was successful, false otherwise.
*/
bool Client::force_disconnect() {
	if (!is_connected()) {
		WARNING("Client is not connected, cannot force disconnect");
		return false;
	}

	enet_peer_disconnect(peers.server_peer, 0); // Send disconnect packet to server just to let it know
	enet_peer_reset(peers.server_peer); // Reset the server peer.
	peers.clear(); // Clear peerlist
	
	INFO("Client forcefully disconnected from server");
	return true;

}