#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include "Networking/NetworkUser.h"
#include "Networking/Server/ServerPeerlist.h"
#include <future>
#include <optional>
#include <chrono>
#include "OpenServer.h"

// Struct for tracking pending connections (awaiting ConnectionInitiation)
struct PendingConnection {
	ENetPeer* peer;
	std::chrono::steady_clock::time_point connect_time;
};

// Inherit NetworkUser
// and allow shared ptrs to be created from this class. 
//	This is very useful for referencing the server instance in async operations
class Server : public NetworkUser, public std::enable_shared_from_this<Server> {
public:
	ServerPeerlist peers; // Server's peerlist
	OpenServer server_info; // Information about the running server.

	Server(const std::string& address, int port);
	~Server();

	std::future<bool> disconnect_peer(uint16_t peer_id);
	std::future<bool> disconnect_all();

	bool send_packet(Packet& packet, uint16_t peer_id);
	bool send_packet_to_peer(Packet& packet, ENetPeer* peer); // Send to ENetPeer directly (for pending connections)
	bool broadcast_packet(Packet& packet, const std::optional<uint16_t>& exclude_peer_id = std::nullopt);

	void start(); // Start the server networking loop
	void stop();  // Stop the server networking loop
	void update(); // Update the server networking state

	// Connection flow handlers
	void handle_connection_initiation(ENetPeer* peer, const std::string& requested_username);

private:
	// Pending connections awaiting ConnectionInitiation packet
	std::unordered_map<ENetPeer*, PendingConnection> pending_connections;
	uint16_t next_peer_id = 1; // Counter for assigning unique peer IDs

	// Helper methods
	void check_pending_connection_timeouts();
	std::string get_unique_username(const std::string& requested_username);
	bool can_accept_new_connection() const;
	std::unordered_map<uint16_t, UserData> get_peers_map() const;
};