#pragma once
#include "Imports/common.h"
#include "Networking/User/UserData.h"
class ClientPeerlist {
public:
	ENetPeer* server_peer = nullptr; // Pointer to the server's ENetPeer
	uint16_t local_server_side_id = 0; // Local user's server-side ID

	ClientPeerlist() = default;

	void clear(); // Clear the peerlist (keeps local_server_side_id)
	void reset(); // Full reset - clear peers AND reset local_server_side_id

	void add_peer(UserData user); // Add a peer to the peerlist

	void remove_peer(uint16_t server_side_id); // Remove a peer from the peerlist by server-side ID
	void remove_peer(const std::string& username); // Remove a peer from the peerlist by username

	void update_peer(const UserData& user); // Update a peer's data in the peerlist

	bool is_local(const UserData& user) const; // Check if a user is the local user
	
	std::vector<UserData> get_all_peers() const; // Get a list of all peers
	std::vector<UserData> get_other_peers() const; // Get a list of all peers except the local user

	std::optional<UserData> get_peer_by_id(uint16_t server_side_id) const; // Get a peer by server-side ID
	std::optional<UserData> get_peer_by_username(const std::string& username) const; // Get a peer by username
	std::optional<UserData> get_local_peer() const; // Get the local user's data

	void set_local_server_side_id(uint16_t server_side_id); // Set the local user's server-side ID
private:
	std::unordered_map<uint16_t, UserData> peers; // Map of server-side ID to UserData
};
