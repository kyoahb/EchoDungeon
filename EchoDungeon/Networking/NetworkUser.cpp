#include "NetworkUser.h"
#include "Utils/NetUtils.h"
NetworkUser::NetworkUser() {}

/*
* @brief Starts the networking loop in a separate thread.
*/
void NetworkUser::start() {
	if (is_running.load())  // Check if the network loop is already running
	{
		WARNING("Networking loop already running, start() call ignored.");
		return;
	}
	is_running.store(true); 

	// Start the networking loop asynchronously in a new thread
	loop_future = std::async(std::launch::async, &NetworkUser::update_loop, this); 
}

/*
* @brief Stops the networking loop. 
* @throws If called from within the networking loop thread, it will log an error and ignore the call to prevent deadlock.
*/
void NetworkUser::stop() {
	if (!is_running.load())  // Check if the network loop is already stopped
	{
		WARNING("Networking loop already stopped, stop() call ignored.");
		return;
	}

	if (std::this_thread::get_id() == thread_id) { // Check for deadlock condition
		ERROR("stop() called from within the networking loop thread, this would cause a deadlock. Ignoring stop() call.");
		return;
	}

	is_running.store(false); 
	if (loop_future.valid()) {
		loop_future.get(); // Wait for the networking loop to finish
	}
}

/*
* @brief The networking loop function that continuously calls the update method.
*/
void NetworkUser::update_loop() {
	thread_id = std::this_thread::get_id(); // Store the thread ID of the networking loop
	while (is_running.load()) {
		update(); // Call the derived class's update method
		std::this_thread::sleep_for(std::chrono::milliseconds(NetworkConstants::LOOP_SLEEP_DURATION_MS)); // Sleep to reduce load
	}
}

/*
* @brief Sends an ENetPacket to a specified ENetPeet.
* @param packet The ENetPacket to send.
* @param peer The ENetPeer to send the packet to.
* @return True if the packet was sent successfully, false otherwise.
*/
bool NetworkUser::send_packet(ENetPacket* packet, ENetPeer* peer) {
	if (!host) { // Check that the local host is valid
		ERROR("Cannot send packet, ENetHost is null.");
		return false;
	}
	
	if (!peer) { // Check that the external peer is valid
		ERROR("Cannot send packet, ENetPeer is null.");
		return false;
	}

	int result = enet_peer_send(peer, 0, packet); // Send the packet on channel 0

	if (result < 0) { // Check for send failure
		ERROR("Failed to send packet to peer with IP address: " + NetUtils::get_ip_string(peer->address));
		return false;
	}
	return true;
}