#include "Server.h"
#include "Networking/Packet/PacketRegistry.h"

/**
 * @brief Constructs a Server instance and initializes the ENet host.
 * @param address The address to bind the server to.
 * @param port The port to bind the server to.
 */
Server::Server(const std::string& address, int port) : NetworkUser(), peers(ServerPeerlist()) {
    // Set server info
    server_info.address = address;
    server_info.port = port;
    server_info.closed = false;

    // Setup ENetAddress
    if (enet_address_set_host(&this->address, address.c_str()) < 0) {
        ERROR("Failed to set server address: " + address);
        return;
    }
    this->address.port = port;

    // Create server host with the specified address, allowing multiple peers
    host = enet_host_create(&this->address, NetworkConstants::MAX_SIMULTANEOUS_CONNECTIONS, 
        NetworkConstants::MAX_CHANNELS, NetworkConstants::BANDWIDTH_LIMIT, NetworkConstants::BANDWIDTH_LIMIT);
    if (host == nullptr) {
        ERROR("Failed to create ENet server host");
        return;
    }

    INFO("Server created at " + address + ":" + std::to_string(port));
}

/**
 * @brief Destructor for Server, cleanly disconnects all peers and stops the networking loop.
 */
Server::~Server() {
    // Disconnect all peers patiently
    if (!peers.get_all_peers().empty()) {
        disconnect_all().get();
    }

    // Stop networking loop if running
    if (is_running.load()) {
        stop();
    }

    // Destroy ENet host
    if (host) {
        enet_host_destroy(host);
        host = nullptr;
    }

    INFO("Server destroyed");
}

/**
 * @brief Disconnects a specific peer from the server.
 * @param peer_id The server-side ID of the peer to disconnect.
 * @return A future that resolves to true if the disconnection was successful, false otherwise.
 */
std::future<bool> Server::disconnect_peer(uint16_t peer_id) {
    // TO BE IMPLEMENTED FULLY

    // FOR NOW, find the peer and force disconnect
    std::optional<PeerEntry> peer_data = peers.get_peer_by_id(peer_id);
    if (!peer_data.has_value()) {
        WARNING("Peer with ID " + std::to_string(peer_id) + " not found, cannot disconnect");
        return std::async(std::launch::deferred, [] { return false; });
    }

    // Built in ENet disconnect
	enet_peer_disconnect(peer_data->peer, 0);

    // Remove from peerlist
	peers.remove_peer(peer_id);

    INFO("Peer " + std::to_string(peer_id) + " disconnected");
    return std::async(std::launch::deferred, [] { return true; });
}

/**
 * @brief Disconnects all peers from the server.
 * @return A future that resolves to true if all disconnections were successful, false otherwise.
 */
std::future<bool> Server::disconnect_all() {
    // TO BE IMPLEMENTED FULLY

    // FOR NOW, clear the peerlist
    peers.clear();

    INFO("All peers disconnected");
    return std::async(std::launch::deferred, [] { return true; });
}

/**
 * @brief Sends a packet to a specific peer.
 * @param packet The packet to send.
 * @param peer_id The server-side ID of the peer to send the packet to.
 * @return true if the packet was sent successfully, false otherwise.
 */
bool Server::send_packet(const Packet& packet, uint16_t peer_id) {
    TRACE("Sending packet " + PacketRegistry::getPacketName(packet.header.type) + " to peer " + std::to_string(peer_id));

	std::optional<PeerEntry> opt_target_peer = peers.get_peer_by_id(peer_id);
    if (!opt_target_peer.has_value()) {
        ERROR("Peer with ID " + std::to_string(peer_id) + " not found in peerlist");
        return false;
	}

    if (opt_target_peer->peer == nullptr) {
        ERROR("Failed to find ENetPeer* for peer ID " + std::to_string(peer_id));
        return false;
    }

    return NetworkUser::send_packet(packet.to_enet_packet(), opt_target_peer->peer);
}

/**
 * @brief Broadcasts a packet to all connected peers, optionally excluding one.
 * @param packet The packet to broadcast.
 * @param exclude_peer_id Optional peer ID to exclude from the broadcast.
 * @return true if the packet was broadcast successfully, false otherwise.
 */
bool Server::broadcast_packet(const Packet& packet, const std::optional<uint16_t>& exclude_peer_id) {
    TRACE("Broadcasting packet " + PacketRegistry::getPacketName(packet.header.type) + " to all peers" + 
          (exclude_peer_id.has_value() ? " (excluding peer " + std::to_string(exclude_peer_id.value()) + ")" : ""));

    bool all_sent = true;
    for (const auto& peer_data : peers.get_all_peers()) {
        // Skip the excluded peer if specified
        if (exclude_peer_id.has_value() && peer_data.data.server_side_id == exclude_peer_id.value()) {
            continue;
        }

        if (!send_packet(packet, peer_data.data.server_side_id)) {
            all_sent = false;
            ERROR("Failed to send packet to peer " + std::to_string(peer_data.data.server_side_id));
        }
    }

    return all_sent;
}

/**
 * @brief Starts the server's networking update loop.
 */
void Server::start() {
    if (is_running) {
        WARNING("Server update loop is already running, cannot start again");
        return;
    }

    if (thread_id == std::this_thread::get_id()) {
        WARNING("Server update loop already exists in this thread, cannot start");
        return;
    }

    NetworkUser::start();

    INFO("Server networking loop started");
    // Trigger server networking loop start event
}

/**
 * @brief Stops the server's networking update loop.
 */
void Server::stop() {
    if (!is_running) {
        WARNING("Server update loop is not running, cannot stop");
        return;
    }

    NetworkUser::stop();

    INFO("Server networking loop stopped");
    // Trigger server networking loop stop event
}

/**
 * @brief Executes a single update cycle for the server, processing incoming events and packets.
 */
void Server::update() {
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                INFO("A new client connected from " + NetUtils::get_ip_string(event.peer->address));
                // TODO: Add peer to peerlist after handshake/authentication
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                // Identify packets, send off events
                // TODO: Deserialize packet and handle it
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT:
                INFO("A client disconnected");
                // TODO: Remove peer from peerlist
                break;

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                INFO("A client disconnected due to timeout");
                // TODO: Remove peer from peerlist
                break;

            default:
                break;
        }
    }
}

