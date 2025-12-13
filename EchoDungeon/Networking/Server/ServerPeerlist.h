#pragma once
#include "Imports/common.h"
#include "Networking/User/UserData.h"

class ServerPeerlist {
public:
    ServerPeerlist() = default;

    void clear(); // Clear the peerlist (used on disconnect)

    void add_peer(ENetPeer* peer, UserData user); // Add a peer to the peerlist

    void remove_peer(ENetPeer* peer); // Remove a peer from the peerlist by ENetPeer
    void remove_peer(uint16_t server_side_id); // Remove a peer from the peerlist by server-side ID
    void remove_peer(const std::string& username); // Remove a peer from the peerlist by username

    void update_peer(const UserData& user); // Update a peer's data in the peerlist
    UserData* get_peer_data(ENetPeer* peer); // Get a pointer to a peer's UserData by ENetPeer

    std::vector<UserData> get_all_peers() const; // Get a list of all peers

    std::optional<UserData> get_peer_by_id(uint16_t server_side_id) const; // Get a peer by server-side ID
    std::optional<UserData> get_peer_by_username(const std::string& username) const; // Get a peer by username
private:
    struct PeerEntry {
        ENetPeer* peer;
        UserData data;
    };
    std::unordered_map<uint16_t, PeerEntry> peers; 
    // Map of server-side ID to PeerEntry (containing ENetPeer* and UserData)
};
