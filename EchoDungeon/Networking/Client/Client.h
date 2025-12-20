#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include "Networking/NetworkConstants.h"
#include "Networking/NetworkUser.h"
#include "Networking/Client/ClientPeerlist.h"
#include <future>
#include <optional>
#include "Networking/Server/OpenServer.h"

// Connection state enum for tracking the connection flow
enum class ClientConnectionState {
	DISCONNECTED,           // Not connected to any server
	CONNECTING,             // ENet connection established, waiting for handshake
	AWAITING_CONFIRMATION,  // Sent ConnectionInitiation, waiting for response
	AWAITING_SERVER_DATA,   // Received ConnectionConfirmation, waiting for ServerDataUpdate
	CONNECTED               // Fully connected and ready
};

// Result struct for connection attempts
struct ConnectionResult {
	bool success = false;
	std::string failure_reason = "";
};

// Inherit NetworkUser
// and allow shared ptrs to be created from this class. 
//	This is very useful for referencing the client instance in async operations
class Client : public NetworkUser, public std::enable_shared_from_this<Client> {
public:
	ClientPeerlist peers; // Client's peerlist
	std::string username;
	OpenServer connected_server_info; // Information about the connected server.
	ClientConnectionState connection_state = ClientConnectionState::DISCONNECTED;

	Client(const std::string& username = "User");
	~Client();

	std::future<ConnectionResult> connect(const std::string& ip, uint16_t port);
	std::future<bool> disconnect(); 
	bool force_disconnect();

	bool send_packet(Packet& packet);

	void start(); // Start the client networking loop
	void stop();  // Stop the client networking loop
	void update(); // Update the client networking state

	bool is_connected() const; // Check if the client is connected to a server

	// Connection flow handlers - called internally when packets are received
	void handle_connection_confirmation(uint16_t assigned_id);
	void handle_connection_refusal(const std::string& reason);
	void handle_server_data_update(const std::unordered_map<uint16_t, UserData>& peers_data, const OpenServer& server_info);

private:
	// Promise for async connection result
	std::optional<std::promise<ConnectionResult>> connection_promise;
};