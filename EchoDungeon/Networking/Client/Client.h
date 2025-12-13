#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include "Networking/NetworkConstants.h"
#include "Networking/NetworkUser.h"
#include <future>

class Client : public std::enable_shared_from_this<Client> {
public:

	Client();
	~Client();

private:
	bool is_connected = false;
	

	std::future<bool> connect(const std::string& address, uint16_t port);
	std::future<bool> disconnect(); 
	bool force_disconnect();

	bool send_packet(const Packet& packet);

	void start(); // Start the client networking loop
	void stop();  // Stop the client networking loop
	void update(); // Update the client networking state

};