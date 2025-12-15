#include "Client.h"
#include "Networking/Packet/PacketRegistry.h"

Client::Client() : NetworkUser(), peers(ClientPeerlist()) {

	// Create client host. No address + 1 max peer since this is a client
	host = enet_host_create(nullptr, 1, NetworkConstants::MAX_CHANNELS, NetworkConstants::BANDWIDTH_LIMIT, NetworkConstants::BANDWIDTH_LIMIT);
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

bool Client::is_connected() const {
	return peers.server_peer != nullptr && peers.server_peer->state == ENET_PEER_STATE_CONNECTED;
}

bool Client::send_packet(const Packet& packet) {
	TRACE("Sending packet " + PacketRegistry::getPacketName(packet.header.type) + " to server");

	if (!is_connected()) {
		ERROR("Client is not connected, cannot send packet");
		return false;
	}

	return NetworkUser::send_packet(packet.to_enet_packet(), peers.server_peer);
}


// UPDATE LOOP

void Client::start() {
	if (!is_connected()) {
		WARNING("Client is not connected, cannot start update loop as it is pointless");
		return;
	}

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

void Client::stop() {
	if (!is_running) {
		WARNING("Client update loop is not running, cannot stop");
		return;
	}

	NetworkUser::stop();

	// Trigger client networking loop stop event
}

void Client::update() {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		// Identify packets, send off events
	}
}