#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include "Networking/NetworkConstants.h"
#include "Networking/NetworkUser.h"
#include "Networking/Client/ClientPeerlist.h"
#include <future>

// Inherit NetworkUser
// and allow shared ptrs to be created from this class. 
//	This is very useful for referencing the client instance in async operations
class Client : public NetworkUser, public std::enable_shared_from_this<Client> {
public:
	ClientPeerlist peers; // Client's peerlist
	std::string username;

	Client(const std::string& username = "User");
	~Client();

	std::future<bool> connect(const std::string& ip, uint16_t port);
	std::future<bool> disconnect(); 
	bool force_disconnect();

	bool send_packet(Packet& packet);

	void start(); // Start the client networking loop
	void stop();  // Stop the client networking loop
	void update(); // Update the client networking state

	bool is_connected() const; // Check if the client is connected to a server
private:

};