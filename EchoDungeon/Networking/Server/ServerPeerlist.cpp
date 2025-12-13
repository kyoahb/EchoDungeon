#include "ServerPeerlist.h"
#include <algorithm>
#include <optional>

/**
 * @brief Clears the peerlist, removing all peers.
 */
void ServerPeerlist::clear() {
    peers.clear();
}

/**
 * @brief Adds a peer to the peerlist.
 * @param peer The ENetPeer pointer.
 * @param user The UserData of the peer.
 */
void ServerPeerlist::add_peer(ENetPeer* peer, UserData user) {
    peers[user.server_side_id] = {peer, user};
}

/**
 * @brief Removes a peer from the peerlist by ENetPeer.
 * @param peer The ENetPeer to remove.
 */
void ServerPeerlist::remove_peer(ENetPeer* peer) {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.peer == peer;
    });
    if (it != peers.end()) {
        peers.erase(it);
    }
}

/**
 * @brief Removes a peer from the peerlist by server-side ID.
 * @param server_side_id The server-side ID of the peer to remove.
 */
void ServerPeerlist::remove_peer(uint16_t server_side_id) {
    peers.erase(server_side_id);
}

/**
 * @brief Removes a peer from the peerlist by username.
 * @param username The username of the peer to remove.
 */
void ServerPeerlist::remove_peer(const std::string& username) {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.data.username == username;
    });
    if (it != peers.end()) {
        peers.erase(it);
    }
}

/**
 * @brief Updates a peer's data in the peerlist.
 * @param user The updated UserData of the peer.
 */
void ServerPeerlist::update_peer(const UserData& user) {
    auto it = peers.find(user.server_side_id);
    if (it != peers.end()) {
        it->second.data = user;
    }
}

/**
 * @brief Gets a pointer to a peer's UserData by ENetPeer.
 * @param peer The ENetPeer to find.
 * @return A pointer to the UserData, or nullptr if not found.
 */
UserData* ServerPeerlist::get_peer_data(ENetPeer* peer) {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.peer == peer;
    });
    if (it != peers.end()) {
        return &it->second.data;
    }
    return nullptr;
}

/**
 * @brief Gets a list of all peers' UserData.
 * @return A vector containing all UserData in the peerlist.
 */
std::vector<UserData> ServerPeerlist::get_all_peers() const {
    std::vector<UserData> all_peers;
    for (const auto& pair : peers) {
        all_peers.push_back(pair.second.data);
    }
    return all_peers;
}

/**
 * @brief Gets a peer by server-side ID.
 * @param server_side_id The server-side ID of the peer.
 * @return An optional containing the UserData if found, or empty otherwise.
 */
std::optional<UserData> ServerPeerlist::get_peer_by_id(uint16_t server_side_id) const {
    auto it = peers.find(server_side_id);
    if (it != peers.end()) {
        return it->second.data;
    }
    return std::nullopt;
}

/**
 * @brief Gets a peer by username.
 * @param username The username of the peer.
 * @return An optional containing the UserData if found, or empty otherwise.
 */
std::optional<UserData> ServerPeerlist::get_peer_by_username(const std::string& username) const {
    auto it = std::find_if(peers.begin(), peers.end(), [&](const auto& pair) {
        return pair.second.data.username == username;
    });
    if (it != peers.end()) {
        return it->second.data;
    }
    return std::nullopt;
}