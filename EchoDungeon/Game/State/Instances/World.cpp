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
			raylib::Vector3{ 1.0f, 1.0f, 1.0f }
		);

		s_world_manager->spawn_enemy(
			100.0f,
			1,
			10.0f,
			"zombie",
			raylib::Vector3{ 10.0f, 1.0f, 10.0f }
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
	if (client_player_spawn_sub != -1) {
		ClientEvents::PlayerSpawnEvent::unregister_callback(client_player_spawn_sub);
		client_player_spawn_sub = -1;
	}
	if (client_player_update_sub != -1) {
		ClientEvents::PlayerUpdateEvent::unregister_callback(client_player_update_sub);
		client_player_update_sub = -1;
	}
	if (client_player_destroy_sub != -1) {
		ClientEvents::PlayerDestroyEvent::unregister_callback(client_player_destroy_sub);
		client_player_destroy_sub = -1;
	}
	if (client_enemy_spawn_sub != -1) {
		ClientEvents::EnemySpawnEvent::unregister_callback(client_enemy_spawn_sub);
		client_enemy_spawn_sub = -1;
	}
	if (client_enemy_update_sub != -1) {
		ClientEvents::EnemyUpdateEvent::unregister_callback(client_enemy_update_sub);
		client_enemy_update_sub = -1;
	}
	if (client_enemy_destroy_sub != -1) {
		ClientEvents::EnemyDestroyEvent::unregister_callback(client_enemy_destroy_sub);
		client_enemy_destroy_sub = -1;
	}
	if (client_object_spawn_sub != -1) {
		ClientEvents::ObjectSpawnEvent::unregister_callback(client_object_spawn_sub);
		client_object_spawn_sub = -1;
	}
	if (client_object_destroy_sub != -1) {
		ClientEvents::ObjectDestroyEvent::unregister_callback(client_object_destroy_sub);
		client_object_destroy_sub = -1;
	}
	if (client_item_pickup_sub != -1) {
		ClientEvents::ItemPickupEvent::unregister_callback(client_item_pickup_sub);
		client_item_pickup_sub = -1;
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
		if (server_player_attack_sub != -1) {
			ServerEvents::PlayerAttackEvent::unregister_callback(server_player_attack_sub);
			server_player_attack_sub = -1;
		}
		if (server_player_disconnect_sub != -1) {
			ServerEvents::DisconnectEvent::unregister_callback(server_player_disconnect_sub);
			server_player_disconnect_sub = -1;
		}
		if (server_player_disconnect_timeout_sub != -1) {
			ServerEvents::DisconnectTimeoutEvent::unregister_callback(server_player_disconnect_timeout_sub);
			server_player_disconnect_timeout_sub = -1;
		}
		if (server_item_discard_sub != -1) {
			ServerEvents::ItemDiscardEvent::unregister_callback(server_item_discard_sub);
			server_item_discard_sub = -1;
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
		return;
	}

	// Update ServerWorldManager (if hosting)
	if (game.is_hosting() && s_world_manager) {
		s_world_manager->update(GetFrameTime());
	}

	if (c_world_manager->get_local_player() == nullptr) {
		// The client has not received a server loading packet yet
		DrawText("Loading world...", 10, 10, 20, BLACK);
	}

	// Check if local player is dead
	if (c_world_manager->get_local_player() != nullptr &&
		c_world_manager->get_local_player()->health <= 0.0f) {
		// Spawn IMGUI window
		ImGui::Begin("You Died", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
			ImGui::Text("You have died. Return to the main menu?");

			if (game.is_hosting()) {
				ImGui::Text("As the host, returning to main menu will end the game for all players.");
			}
			if (ImGui::Button("Return to Main Menu")) {
				should_quit_to_mainmenu.store(true);
			}
		ImGui::End();
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

	// EnemySpawn - New enemy spawned in world
	client_enemy_spawn_sub = ClientEvents::EnemySpawnEvent::register_callback(
		[this](const ClientEvents::EnemySpawnEventData& data) {
			if (c_world_manager) {
				Enemy e = Enemy(data.packet.id, data.packet.max_health, 
					data.packet.speed, data.packet.damage, data.packet.asset_id);
				e.transform = data.packet.transform;
				e.health = data.packet.health;
				
				c_world_manager->add_enemy(e);
				TRACE("Spawned enemy: ID=" + std::to_string(e.id));
			}
		}
	);

	// EnemyUpdate - Enemy updated in world
	client_enemy_update_sub = ClientEvents::EnemyUpdateEvent::register_callback(
		[this](const ClientEvents::EnemyUpdateEventData& data) {
			if (c_world_manager) {
				for (const auto& update : data.packet.updates) {
					c_world_manager->update_enemy(update.id, update.transform, update.health);
					TRACE("Enemy updated: ID=" + std::to_string(update.id)
						+ "transform pos=" + update.transform.get_position().ToString());
				}
			}
		}
	);

	// EnemyDestroy - Enemy removed from world
	client_enemy_destroy_sub = ClientEvents::EnemyDestroyEvent::register_callback(
		[this](const ClientEvents::EnemyDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_enemy(data.packet.id);
				TRACE("Enemy destroyed: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// ObjectSpawn - New object spawned in world
	client_object_spawn_sub = ClientEvents::ObjectSpawnEvent::register_callback(
		[this](const ClientEvents::ObjectSpawnEventData& data) {
			if (c_world_manager) {
				Object o = Object(data.packet.id, data.packet.asset_id, data.packet.object_type);
				o.transform = data.packet.transform;

				c_world_manager->add_object(o);
				TRACE("Spawned object: ID=" + std::to_string(o.id));
			}
		}
	);

	// ObjectDestroy - Object removed from world
	client_object_destroy_sub = ClientEvents::ObjectDestroyEvent::register_callback(
		[this](const ClientEvents::ObjectDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_object(data.packet.id);
				TRACE("Destroyed object: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// PlayerUpdate - New object spawned in world
	client_player_update_sub = ClientEvents::PlayerUpdateEvent::register_callback(
		[this](const ClientEvents::PlayerUpdateEventData& data) {
			if (c_world_manager) {
				for (const auto& update : data.packet.updates) {
					c_world_manager->update_player(update.id, update.transform, update.health,
						update.damage, update.max_health, update.range, update.speed,
						update.attack_cooldown, update.last_attack_time, update.attacking,
						update.inventory
					);
					TRACE("Player updated: ID=" + std::to_string(update.id));
				}
			}
		}
	);
	
	// PlayerDestroy - Player removed from world
	client_player_destroy_sub = ClientEvents::PlayerDestroyEvent::register_callback(
		[this](const ClientEvents::PlayerDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_player(data.packet.id);
				TRACE("Player removed: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// PlayerSpawn - New player joined the world
	client_player_spawn_sub = ClientEvents::PlayerSpawnEvent::register_callback(
		[this](const ClientEvents::PlayerSpawnEventData& data) {
			if (c_world_manager) {
				Player p = Player(data.packet.id, false, data.packet.name);
				p.damage = data.packet.damage;
				p.health = data.packet.health;
				p.max_health = data.packet.max_health;
				p.speed = data.packet.speed;
				p.transform = data.packet.transform;
				p.attack_cooldown = data.packet.attack_cooldown;
				p.range = data.packet.range;
				p.last_attack_time = data.packet.last_attack_time;

				c_world_manager->add_player(p);
				TRACE("Player spawned: " + p.name + " ID=" + std::to_string(p.id));
			}
		}
	);
	
	// ItemPickup - Player picked up an item
	client_item_pickup_sub = ClientEvents::ItemPickupEvent::register_callback(
		[this](const ClientEvents::ItemPickupEventData& data) {
			if (c_world_manager) {
				c_world_manager->handle_item_pickup(
					data.packet.player_id,
					data.packet.item
				);
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

	// PlayerAttack - Client attacks
	server_player_attack_sub = ServerEvents::PlayerAttackEvent::register_callback(
		[this](const ServerEvents::PlayerAttackEventData& data) {
			if (s_world_manager) {
				auto peer_opt = game.server->peers.get_peer_by_enet(data.peer);
				if (peer_opt.has_value()) {
					uint16_t peer_id = peer_opt.value().data.server_side_id;
					s_world_manager->handle_player_attack(peer_id);
					TRACE("Player attacked: ID=" + std::to_string(peer_id));
				}
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
	
	// ItemDiscard - Player discards an item
	server_item_discard_sub = ServerEvents::ItemDiscardEvent::register_callback(
		[this](const ServerEvents::ItemDiscardEventData& data) {
			if (s_world_manager) {
				auto peer_opt = game.server->peers.get_peer_by_enet(data.peer);
				if (!peer_opt.has_value()) return;
				s_world_manager->handle_item_discard(
					peer_opt.value().data.server_side_id,
					data.packet.item_id
				);
			}
		}
	);
}