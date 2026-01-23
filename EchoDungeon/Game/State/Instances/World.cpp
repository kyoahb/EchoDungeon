#include "World.h"
#include "Game/Game.h"
#include "Utils/Input.h"
#include "Game/Events/EventList.h"
#include "Networking/Packet/Instances/RequestWorldSnapshot.h"

World::World(Game& game) : GameState(game) {
	// Player is created in on_activate when client is available
}

void World::on_activate() {
	TRACE("World activated");

	// Create ClientWorldManager (always, for all clients)
	c_world_manager = std::make_unique<ClientWorldManager>(game.client);
	setup_client_events();

	// Create ServerWorldManager (only if hosting)
	if (game.is_hosting()) {
		s_world_manager = std::make_unique<ServerWorldManager>(game.server);
		setup_server_events();

		TRACE("ServerWorldManager created");

		// Add all existing connected players to the world
		auto all_peers = game.server->peers.get_all_peers();
		for (const auto& peer_entry : all_peers) {
			s_world_manager->add_player(peer_entry.data.server_side_id, peer_entry.data.username);
		}

		s_world_manager->spawn_object(
			ObjectType::MODEL,
			"cube",
			raylib::Vector3{ 1.0f, 0.5f, 1.0f }
		);

		// Broadcast initial world snapshot to all clients
		s_world_manager->broadcast_world_snapshot();
	}

	// If not hosting, request world snapshot from server
	if (!game.is_hosting()) {
		RequestWorldSnapshotPacket request;
		game.client->send_packet(request);
		TRACE("Requested world snapshot from server");
	}
}

void World::on_deactivate() {
	TRACE("World deactivated");
	
	// Unsubscribe from client events
	if (client_world_snapshot_sub != -1) {
		ClientEvents::WorldSnapshotEvent::unregister_callback(client_world_snapshot_sub);
		client_world_snapshot_sub = -1;
	}
	if (client_entity_update_sub != -1) {
		ClientEvents::EntityUpdateEvent::unregister_callback(client_entity_update_sub);
		client_entity_update_sub = -1;
	}
	if (client_entity_spawn_sub != -1) {
		ClientEvents::EntitySpawnEvent::unregister_callback(client_entity_spawn_sub);
		client_entity_spawn_sub = -1;
	}
	if (client_entity_destroy_sub != -1) {
		ClientEvents::EntityDestroyEvent::unregister_callback(client_entity_destroy_sub);
		client_entity_destroy_sub = -1;
	}
	if (client_player_spawn_sub != -1) {
		ClientEvents::PlayerSpawnEvent::unregister_callback(client_player_spawn_sub);
		client_player_spawn_sub = -1;
	}
	
	// Unsubscribe from server events (if hosting)
	if (game.is_hosting()) {
		if (server_player_input_sub != -1) {
			ServerEvents::PlayerInputEvent::unregister_callback(server_player_input_sub);
			server_player_input_sub = -1;
		}
		if (server_request_snapshot_sub != -1) {
			ServerEvents::RequestWorldSnapshotEvent::unregister_callback(server_request_snapshot_sub);
			server_request_snapshot_sub = -1;
		}
		if (server_player_disconnect_sub != -1) {
			ServerEvents::DisconnectEvent::unregister_callback(server_player_disconnect_sub);
			server_player_disconnect_sub = -1;
		}
		if (server_player_disconnect_timeout_sub != -1) {
			ServerEvents::DisconnectTimeoutEvent::unregister_callback(server_player_disconnect_timeout_sub);
			server_player_disconnect_timeout_sub = -1;
		}
	}
	
	// Clear and destroy ClientWorldManager
	if (c_world_manager) {
		c_world_manager->clear();
		c_world_manager.reset();
		c_world_manager = nullptr;
	}
	
	// Clear and destroy ServerWorldManager (if hosting)
	if (s_world_manager) {
		s_world_manager->clear();
		s_world_manager.reset();
		s_world_manager = nullptr;
	}
}

void World::update() {
	if (!c_world_manager) return;

	if (should_quit_to_mainmenu.load()) {
		TRACE("Quit flag set in world state, returning to main menu");
		should_quit_to_mainmenu.store(false);

		// Destroy server if it exists
		if (game.server) {
			game.server->stop().get();
			game.server = nullptr;
		}

		// Destroy client if it exists
		if (game.client) {
			game.client->disconnect("Quit flag set in world state").get();
			game.client->stop();
			game.client = nullptr;
		}

		game.state_manager.set_state("MainMenu");
		return;
	}

	// Set exit flag if disconnected
	if (!game.client->is_connected()) {
		should_quit_to_mainmenu.store(true);
	}

	// Update ServerWorldManager (if hosting)
	if (game.is_hosting() && s_world_manager) {
		s_world_manager->update(GetFrameTime());
	}

	if (c_world_manager->get_local_player() == nullptr) {
		// The client has not received a server loading packet yet
		DrawText("Loading world...", 10, 10, 20, BLACK);
	}
	
	// Update ClientWorldManager
	// This updates input, camera, sends input packets 
	c_world_manager->update(GetFrameTime());


	c_world_manager->get_camera().BeginMode();
	// Do all 3D Rendering
	DrawGrid(100, 1.0f);
	
	c_world_manager->draw_3d();

	// End 3D Rendering
	c_world_manager->get_camera().EndMode();


	// Do all 2D Rendering
	c_world_manager->draw_2d();

	// DEBUG: Draw hud info
	DrawFPS(10, 0);

	const std::vector<int>& keys = Input::get_keys_down();
	std::string keysText = "Pressed keys: ";
	for (int key : keys) {
		keysText += Input::get_key_name(key) + ", ";
	}
	DrawText(keysText.c_str(), 10, 80, 20, BLACK);

	// End 2D Rendering
}

void World::setup_client_events() {
	// WorldSnapshot - Receive full world state
	client_world_snapshot_sub = ClientEvents::WorldSnapshotEvent::register_callback(
		[this](const ClientEvents::WorldSnapshotEventData& data) {
			if (c_world_manager) {
				c_world_manager->apply_world_snapshot(data.packet);
				TRACE("Applied world snapshot");
			}
		}
	);
	
	// EntityUpdate - Receive entity position/health updates
	client_entity_update_sub = ClientEvents::EntityUpdateEvent::register_callback(
		[this](const ClientEvents::EntityUpdateEventData& data) {
			if (c_world_manager) {
				c_world_manager->apply_entity_updates(data.packet.updates);
			}
		}
	);
	
	// EntitySpawn - New object spawned in world
	client_entity_spawn_sub = ClientEvents::EntitySpawnEvent::register_callback(
		[this](const ClientEvents::EntitySpawnEventData& data) {
				if (c_world_manager) {
					c_world_manager->add_object(data.packet.object);
					TRACE("Entity spawned: ID=" + std::to_string(data.packet.object.id));
				}
		}
	);
	
	// EntityDestroy - Object/player removed from world
	client_entity_destroy_sub = ClientEvents::EntityDestroyEvent::register_callback(
		[this](const ClientEvents::EntityDestroyEventData& data) {
				if (c_world_manager) {
					if (data.packet.entity_type == EntityType::PLAYER) {
						c_world_manager->remove_player(data.packet.entity_id);
						TRACE("Player removed: ID=" + std::to_string(data.packet.entity_id));
					}
					else if (data.packet.entity_type == EntityType::OBJECT) {
						c_world_manager->remove_object(data.packet.entity_id);
						TRACE("Object removed: ID=" + std::to_string(data.packet.entity_id));
					}
				}
		}
	);
	
	// PlayerSpawn - New player joined the world
	client_player_spawn_sub = ClientEvents::PlayerSpawnEvent::register_callback(
		[this](const ClientEvents::PlayerSpawnEventData& data) {
			if (c_world_manager) {
				c_world_manager->add_player(data.packet.player);
				TRACE("Player spawned: " + data.packet.player.name + " ID=" + std::to_string(data.packet.player.id));
			}
		}
	);
}

void World::setup_server_events() {
	// PlayerInput - Client sends their movement/transform
	server_player_input_sub = ServerEvents::PlayerInputEvent::register_callback(
		[this](const ServerEvents::PlayerInputEventData& data) {
			if (s_world_manager) {
				auto peer_opt = game.server->peers.get_peer_by_enet(data.peer);
				if (peer_opt.has_value()) {
					uint16_t peer_id = peer_opt.value().data.server_side_id;
					s_world_manager->handle_player_input(peer_id, data.packet.transform);
				}
			}
		}
	);
	
	// RequestWorldSnapshot - Client requests full world state
	server_request_snapshot_sub = ServerEvents::RequestWorldSnapshotEvent::register_callback(
		[this](const ServerEvents::RequestWorldSnapshotEventData& data) {
			if (s_world_manager) {
				// Send world snapshot to the requesting client only
				s_world_manager->send_world_snapshot(data.peer);
				TRACE("Sent world snapshot to peer");
			}
		}
	);

	// Player disconnect - Remove player from world
	server_player_disconnect_sub = ServerEvents::DisconnectEvent::register_callback(
		[this](const ServerEvents::DisconnectEventData& data) {
			if (s_world_manager) {
				auto peer_opt = game.server->peers.get_peer_by_enet(data.event.peer);
				if (peer_opt.has_value()) {
					uint16_t peer_id = peer_opt.value().data.server_side_id;
					s_world_manager->remove_player(peer_id);
					TRACE("Removed disconnected player: ID=" + std::to_string(peer_id));
				}
			}
		}
	);

	// Player disconnect timeout - Remove player from world
	server_player_disconnect_timeout_sub = ServerEvents::DisconnectTimeoutEvent::register_callback(
		[this](const ServerEvents::DisconnectTimeoutEventData& data) {
			if (s_world_manager) {
				auto peer_opt = game.server->peers.get_peer_by_enet(data.event.peer);
				if (peer_opt.has_value()) {
					uint16_t peer_id = peer_opt.value().data.server_side_id;
					s_world_manager->remove_player(peer_id);
					TRACE("Removed timed-out player: ID=" + std::to_string(peer_id));
				}
			}
		}
	);
}