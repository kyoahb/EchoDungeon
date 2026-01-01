#include "Server.h"
#include "Networking/Packet/PacketRegistry.h"
#include "Networking/Packet/Instances/ConnectionInitiation.h"
#include "Networking/Packet/Instances/ConnectionConfirmation.h"
#include "Networking/Packet/Instances/ConnectionRefusal.h"
#include "Networking/Packet/Instances/ServerDataUpdate.h"
#include "Game/Events/EventList.h"
#include "Networking/Packet/Instances/DisconnectInfo.h"
#include "Networking/Packet/Instances/DisconnectKick.h"

// Timeout for pending connections (1 minute)
constexpr auto PENDING_CONNECTION_TIMEOUT = std::chrono::seconds(60);

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
	server_info.external_address = NetUtils::get_external_ip_string();

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

    // Register event callback for ConnectionInitiation packets
    on_connection_initiation_callback = ServerEvents::ConnectionInitiationEvent::register_callback(
        [this](const ServerEvents::ConnectionInitiationEventData& data) {
            handle_connection_initiation(data.peer, data.packet.client_preferred_username);
        }
    );
    on_general_information_update_callback = ServerEvents::GeneralInformationUpdateEvent::register_callback(
        [this](const ServerEvents::GeneralInformationUpdateEventData& data) {
            handle_general_information_update(data.peer, data.packet);
        }
    );

    INFO("Server created at " + address + ":" + std::to_string(port));
}

/**
 * @brief Destructor for Server, cleanly disconnects all peers and stops the networking loop.
 */
Server::~Server() {
    // Unregister event callbacks
    ServerEvents::ConnectionInitiationEvent::unregister_callback(on_connection_initiation_callback);
	ServerEvents::GeneralInformationUpdateEvent::unregister_callback(on_general_information_update_callback);

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

// ============================================================================
// PEER MANAGEMENT
// ============================================================================

/**
 * @brief Disconnects a specific peer from the server.
 * @param peer_id The server-side ID of the peer to disconnect.
 * @return A future that resolves to true if the disconnection was successful.
 */
std::future<bool> Server::disconnect_peer(uint16_t peer_id, const std::string& reason) {
    return std::async(std::launch::async, [this, peer_id, reason]() {
        std::optional<PeerEntry> peer_data = peers.get_peer_by_id(peer_id);
        if (!peer_data.has_value()) {
            WARNING("Peer with ID " + std::to_string(peer_id) + " not found, cannot disconnect");
            return false;
        }

        // Send a disconnect kick packet
        auto packet = DisconnectKickPacket(reason);
        send_packet(packet, peer_id);

        // Disconnect a peer
        enet_peer_disconnect(peer_data->peer, 0);
        peers.remove_peer(peer_id);

        INFO("Peer " + std::to_string(peer_id) + " kicked for reason: " + reason);
        return true;
    });
}

/**
 * @brief Disconnects all peers from the server.
 * @return A future that resolves to true if all disconnections were successful.
 */
std::future<bool> Server::disconnect_all(const std::string& reason) {
    std::vector<std::future<bool>> disconnect_futures;

    for (const auto& peer_entry : peers.get_all_peers()) {
        if (peer_entry.peer) {
            disconnect_futures.push_back(disconnect_peer(peer_entry.data.server_side_id, reason));
        }
    }

    return std::async(std::launch::async, [this, disconnect_futures = std::move(disconnect_futures)]() mutable {
        // Wait for all disconnections to complete
        bool all_successful = true;
        for (auto& future : disconnect_futures) {
            if (!future.get()) {
                all_successful = false;
            }
        }

        // Clear remaining data after disconnections
        peers.clear();
        pending_connections.clear();

        INFO("All peers disconnected");
        return all_successful;
    });
}

// ============================================================================
// PACKET SENDING
// ============================================================================

/**
 * @brief Sends a packet to a specific peer by ID.
 * @param packet The packet to send.
 * @param peer_id The server-side ID of the peer to send the packet to.
 * @return true if the packet was sent successfully, false otherwise.
 */
bool Server::send_packet(Packet& packet, uint16_t peer_id) {
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
 * @brief Sends a packet directly to an ENetPeer (for pending connections not yet in peerlist).
 * @param packet The packet to send.
 * @param peer The ENetPeer to send to.
 * @return true if the packet was sent successfully, false otherwise.
 */
bool Server::send_packet_to_peer(Packet& packet, ENetPeer* peer) {
    TRACE("Sending packet " + PacketRegistry::getPacketName(packet.header.type) + " to ENetPeer");
    
    if (peer == nullptr) {
        ERROR("Cannot send packet to null peer");
        return false;
    }

    return NetworkUser::send_packet(packet.to_enet_packet(), peer);
}

/**
 * @brief Broadcasts a packet to all connected peers, optionally excluding one.
 * @param packet The packet to broadcast.
 * @param exclude_peer_id Optional peer ID to exclude from the broadcast.
 * @return true if the packet was broadcast successfully, false otherwise.
 */
bool Server::broadcast_packet(Packet& packet, const std::optional<uint16_t>& exclude_peer_id) {
    TRACE("Broadcasting packet " + PacketRegistry::getPacketName(packet.header.type) + " to all peers" + 
          (exclude_peer_id.has_value() ? " (excluding peer " + std::to_string(exclude_peer_id.value()) + ")" : ""));

    bool all_sent = true;
    for (const auto& peer_data : peers.get_all_peers()) {
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

// ============================================================================
// UPDATE LOOP
// ============================================================================

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
}

/**
 * @brief Stops the server's networking update loop.
 */
std::future<void> Server::stop() {
    return std::async(std::launch::async, [this]() {
        if (!is_running) {
            WARNING("Server update loop is not running, cannot stop");
            return;
        }

        // Disconnect all users patiently
        disconnect_all("Server shutting down").get();

        NetworkUser::stop();
        INFO("Server networking loop stopped");
    });
}

/**
 * @brief Executes a single update cycle for the server, processing incoming events and packets.
 */
void Server::update() {
    // Check for timed out pending connections
    check_pending_connection_timeouts();
    
    ENetEvent event;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                INFO("A new client connected from " + NetUtils::get_ip_string(event.peer->address));
                
                // Add to pending connections - waiting for ConnectionInitiation packet
                pending_connections[event.peer] = {
                    event.peer,
                    std::chrono::steady_clock::now()
                };
                
                // Trigger event for external listeners
                ServerEvents::ConnectionEventData data(event);
                ServerEvents::ConnectionEvent::trigger(data);
                break;
            }

            case ENET_EVENT_TYPE_RECEIVE:
                // Deserialize and trigger packet event
                PacketRegistry::handleServerPacket(event.peer, event.packet);
                enet_packet_destroy(event.packet);
                break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                INFO("A client disconnected");
                
                // Remove from pending connections if applicable
                pending_connections.erase(event.peer);
                
                // Remove from peerlist if applicable
                peers.remove_peer(event.peer);

				// Broadcast ServerDataUpdate to all remaining peers
				auto update_packet = ServerDataUpdatePacket(get_peers_map(), server_info);
				broadcast_packet(update_packet);
                
                // Trigger event for external listeners
                ServerEvents::DisconnectEventData data(event);
                ServerEvents::DisconnectEvent::trigger(data);
                break;
            }

            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT: {
                INFO("A client disconnected due to timeout");
                
                // Remove from pending connections if applicable
                pending_connections.erase(event.peer);
                
                // Remove from peerlist if applicable
                peers.remove_peer(event.peer);

                // Broadcast ServerDataUpdate to all remaining peers
                auto update_packet = ServerDataUpdatePacket(get_peers_map(), server_info);
                broadcast_packet(update_packet);
                
                // Trigger event for external listeners
                ServerEvents::DisconnectTimeoutEventData data(event);
                ServerEvents::DisconnectTimeoutEvent::trigger(data);
                break;
            }

            default:
                break;
        }
    }
}

// ============================================================================
// CONNECTION FLOW HANDLERS
// ============================================================================

/**
 * @brief Handles a ConnectionInitiation packet from a client.
 * @param peer The ENetPeer that sent the packet.
 * @param requested_username The username requested by the client.
 */
void Server::handle_connection_initiation(ENetPeer* peer, const std::string& requested_username) {
    INFO("Received ConnectionInitiation from peer, requested username: " + requested_username);
    
    // Check if this peer is in pending connections
    auto pending_it = pending_connections.find(peer);
    if (pending_it == pending_connections.end()) {
        WARNING("Received ConnectionInitiation from peer not in pending connections");
        return;
    }
    
    // Remove from pending connections
    pending_connections.erase(pending_it);
    
    // Check if we can accept new connections
    if (!can_accept_new_connection()) {
        INFO("Rejecting connection: server is full or not accepting new connections");
        auto refusal_packet = ConnectionRefusalPacket("Server is full or not accepting new connections");
        send_packet_to_peer(refusal_packet, peer);
        enet_peer_disconnect_later(peer, 0);
        return;
    }
    
    // Generate unique username
    std::string final_username = get_unique_username(requested_username);
    if (final_username != requested_username) {
        INFO("Username '" + requested_username + "' taken, assigned '" + final_username + "'");
    }
    
    // Assign peer ID
    uint16_t peer_id = next_peer_id++;
    
    // Create UserData for this peer
    UserData user_data;
    user_data.server_side_id = peer_id;
    user_data.username = final_username;
    user_data.is_host = (peers.get_all_peers().empty()); // First peer is host
    user_data.ip_address = NetUtils::get_ip_string(peer->address);
    user_data.connected_at = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    user_data.last_packet_time = user_data.connected_at;
    user_data.status = UserStatus::CONNECTED;
	user_data.current_state = "Lobby"; // Just assume Lobby on connect
    
    // Add to peerlist
    peers.add_peer(peer, user_data);
    
    // Send ConnectionConfirmation with assigned ID
    auto confirm_packet = ConnectionConfirmationPacket(peer_id);
    send_packet_to_peer(confirm_packet, peer);
    
    // Send ServerDataUpdate to all peers (including the new one)
    auto update_packet = ServerDataUpdatePacket(get_peers_map(), server_info);
    broadcast_packet(update_packet);
    
    INFO("Peer " + std::to_string(peer_id) + " (" + final_username + ") fully connected");
}

// ============================================================================
// HELPER METHODS
// ============================================================================

/**
 * @brief Checks for and handles timed out pending connections.
 */
void Server::check_pending_connection_timeouts() {
    auto now = std::chrono::steady_clock::now();
    
    std::vector<ENetPeer*> timed_out_peers;
    
    for (const auto& [peer, pending] : pending_connections) {
        if (now - pending.connect_time > PENDING_CONNECTION_TIMEOUT) {
            timed_out_peers.push_back(peer);
        }
    }
    
    for (ENetPeer* peer : timed_out_peers) {
        WARNING("Pending connection timed out - no ConnectionInitiation received within 60 seconds");
        pending_connections.erase(peer);
        enet_peer_disconnect_now(peer, 0);
    }
}

/**
 * @brief Generates a unique username, adding suffix if necessary.
 * @param requested_username The originally requested username.
 * @return A unique username (possibly with _1, _2, etc. suffix).
 */
std::string Server::get_unique_username(const std::string& requested_username) {
    std::string base_username = requested_username;
    std::string final_username = base_username;
    int suffix = 1;
    
    while (peers.get_peer_by_username(final_username).has_value()) {
        final_username = base_username + "_" + std::to_string(suffix);
        suffix++;
    }
    
    return final_username;
}

/**
 * @brief Checks if the server can accept a new connection.
 * @return true if new connections are allowed, false otherwise.
 */
bool Server::can_accept_new_connection() const {
    if (server_info.closed) {
        return false;
    }
    
    if (peers.get_all_peers().size() >= server_info.max_players) {
        return false;
    }
    
    return true;
}

/**
 * @brief Gets the current peers as an unordered_map for packet serialization.
 * @return Map of peer ID to UserData.
 */
std::unordered_map<uint16_t, UserData> Server::get_peers_map() const {
    std::unordered_map<uint16_t, UserData> result;
    for (const auto& entry : peers.get_all_peers()) {
        result[entry.data.server_side_id] = entry.data;
    }
    return result;
}

// ============================================================================
// MISC PACKET HANDLERS
// ============================================================================

/**
 * @brief Handles a GeneralInformationUpdate packet from a client.
 * @param peer The ENetPeer that sent the packet.
 * @param data The GeneralInformationUpdate packet data.
 */
void Server::handle_general_information_update(ENetPeer* peer, const GeneralInformationUpdatePacket& packet) {
    std::optional<PeerEntry> opt_peer_entry = peers.get_peer_by_enet(peer);
    if (!opt_peer_entry.has_value()) {
        WARNING("Received GeneralInformationUpdate from unknown peer");
        return;
    }
    // Update the peer's current state
    PeerEntry& peer_entry = opt_peer_entry.value();
    peer_entry.data.current_state = packet.current_state;
    TRACE("Updated peer " + std::to_string(peer_entry.data.server_side_id) + " current state to " + packet.current_state);
}