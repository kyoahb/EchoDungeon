#include "ClientPeerlist.h"
#include <algorithm>

/**
 * @brief Clears the peerlist, removing all peers and resetting the local server-side ID.
 */
void ClientPeerlist::clear() {
    peers.clear();
	// local_server_side_id = 0; Caused an issue so we no longer reset this
}

/**
 * @brief Adds a peer to the peerlist.
 * @param user The UserData of the peer to add.
 */
void ClientPeerlist::add_peer(UserData user) {
    peers[user.server_side_id] = user;
}

/**
 * @brief Removes a peer from the peerlist by server-side ID.
 * @param server_side_id The server-side ID of the peer to remove.
 */
void ClientPeerlist::remove_peer(uint16_t server_side_id) {
    peers.erase(server_side_id);
}

/**
 * @brief Removes a peer from the peerlist by username.
 * @param username The username of the peer to remove.
 */
void ClientPeerlist::remove_peer(const std::string& username) {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.username == username;
    });
    if (it != peers.end()) {
        peers.erase(it);
    }
}

/**
 * @brief Updates a peer's data in the peerlist.
 * @param user The updated UserData of the peer.
 */
void ClientPeerlist::update_peer(const UserData& user) {
    peers[user.server_side_id] = user;
}

/**
 * @brief Checks if a user is the local user.
 * @param user The UserData to check.
 * @return True if the user is the local user, false otherwise.
 */
bool ClientPeerlist::is_local(const UserData& user) const {
    return user.server_side_id == local_server_side_id;
}

/**
 * @brief Gets a list of all peers.
 * @return A vector containing all UserData in the peerlist.
 */
std::vector<UserData> ClientPeerlist::get_all_peers() const {
    std::vector<UserData> all_peers;
    for (const auto& pair : peers) {
        all_peers.push_back(pair.second);
    }
    return all_peers;
}

/**
 * @brief Gets a list of all peers except the local user.
 * @return A vector containing UserData of all peers except the local user.
 */
std::vector<UserData> ClientPeerlist::get_other_peers() const {
    std::vector<UserData> other_peers;
    for (const auto& pair : peers) {
        if (pair.first != local_server_side_id) {
            other_peers.push_back(pair.second);
        }
    }
    return other_peers;
}

/**
 * @brief Gets a peer by server-side ID.
 * @param server_side_id The server-side ID of the peer.
 * @return The UserData of the peer, or none if not found.
 */
std::optional<UserData> ClientPeerlist::get_peer_by_id(uint16_t server_side_id) const {
    auto it = peers.find(server_side_id);
    if (it != peers.end()) {
        return it->second;
    }
    return std::nullopt;
}

/**
 * @brief Gets a peer by username.
 * @param username The username of the peer.
 * @return The UserData of the peer, or none if not found
 */
std::optional<UserData> ClientPeerlist::get_peer_by_username(const std::string& username) const {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.username == username;
    });
    if (it != peers.end()) {
        return it->second;
    }
    return std::nullopt;
}

/**
 * @brief Gets the local user's data.
 * @return The UserData of the local user, or none if not set
 */
std::optional<UserData> ClientPeerlist::get_local_peer() const {
    return get_peer_by_id(local_server_side_id);
}

/**
 * @brief Sets the local user's server-side ID.
 * @param server_side_id The server-side ID to set.
 */
void ClientPeerlist::set_local_server_side_id(uint16_t server_side_id) {
    local_server_side_id = server_side_id;
}