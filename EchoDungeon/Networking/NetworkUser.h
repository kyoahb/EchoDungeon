#pragma once
#include "Imports/common.h"
#include "Networking/Packet/Packet.h"
#include <future>
#include "Networking/NetworkConstants.h"

class NetworkUser {
public:
	NetworkUser();
	
	ENetAddress address = { 0 }; // Local address
	ENetHost* host = nullptr; // Local ENet host

	void start(); // Start the networking loop
	void stop();  // Stop the networking loop
	bool send_packet(ENetPacket* packet, ENetPeer* peer); // Send a packet to a peer

protected:
	std::atomic<bool> is_running = false; // Is the networking loop running
	std::future<void> loop_future; // Future for the networking loop
	std::thread::id thread_id; // ID of the networking loop thread

	virtual void update() = 0; // Update the networking state. This should be defined in the derived class
	void update_loop(); // The networking loop function
};