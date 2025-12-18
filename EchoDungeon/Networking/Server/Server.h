#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include "Networking/NetworkUser.h"
#include "Networking/Server/ServerPeerlist.h"
#include <future>
#include <optional>

// Struct defining a running server
struct OpenServer {
	std::string address = ""; // Server address
	uint16_t port = 0;        // Server port
	bool closed = false;    // Is the server denying new connections
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

	bool send_packet(const Packet& packet, uint16_t peer_id);
	bool broadcast_packet(const Packet& packet, const std::optional<uint16_t>& exclude_peer_id = std::nullopt);

	void start(); // Start the server networking loop
	void stop();  // Stop the server networking loop
	void update(); // Update the server networking state
private:

};