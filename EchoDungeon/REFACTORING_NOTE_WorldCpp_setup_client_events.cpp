// This is what World.cpp setup_client_events() should look like:

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
	
	// PlayerSpawn - New player joined the world
	client_player_spawn_sub = ClientEvents::PlayerSpawnEvent::register_callback(
		[this](const ClientEvents::PlayerSpawnEventData& data) {
			if (c_world_manager) {
				// Create Player from packet fields
				Player player(data.packet.id, false, data.packet.name);
				player.transform = data.packet.transform;
				player.health = data.packet.health;
				player.max_health = data.packet.max_health;
				player.damage = data.packet.damage;
				player.range = data.packet.range;
				player.speed = data.packet.speed;
				player.asset_id = data.packet.asset_id;
				
				c_world_manager->add_player(player);
				TRACE("Player spawned: " + data.packet.name + " ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// PlayerUpdate - Receive player position/health updates
	client_player_update_sub = ClientEvents::PlayerUpdateEvent::register_callback(
		[this](const ClientEvents::PlayerUpdateEventData& data) {
			if (c_world_manager) {
				c_world_manager->apply_player_updates(data.packet.updates);
			}
		}
	);
	
	// PlayerDestroy - Player disconnected or removed
	client_player_destroy_sub = ClientEvents::PlayerDestroyEvent::register_callback(
		[this](const ClientEvents::PlayerDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_player(data.packet.id);
				TRACE("Player removed: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// EnemySpawn - New enemy spawned in world
	client_enemy_spawn_sub = ClientEvents::EnemySpawnEvent::register_callback(
		[this](const ClientEvents::EnemySpawnEventData& data) {
			if (c_world_manager) {
				// Create Enemy from packet fields
				Enemy enemy(data.packet.id, data.packet.max_health, data.packet.speed,
					data.packet.damage, data.packet.asset_id);
				enemy.transform = data.packet.transform;
				enemy.health = data.packet.health;
				
				c_world_manager->add_enemy(enemy);
				TRACE("Enemy spawned: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// EnemyUpdate - Receive enemy position/health updates
	client_enemy_update_sub = ClientEvents::EnemyUpdateEvent::register_callback(
		[this](const ClientEvents::EnemyUpdateEventData& data) {
			if (c_world_manager) {
				c_world_manager->apply_enemy_updates(data.packet.updates);
			}
		}
	);
	
	// EnemyDestroy - Enemy killed or removed
	client_enemy_destroy_sub = ClientEvents::EnemyDestroyEvent::register_callback(
		[this](const ClientEvents::EnemyDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_enemy(data.packet.id);
				TRACE("Enemy removed: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// ObjectSpawn - New object spawned in world
	client_object_spawn_sub = ClientEvents::ObjectSpawnEvent::register_callback(
		[this](const ClientEvents::ObjectSpawnEventData& data) {
			if (c_world_manager) {
				// Create Object from packet fields
				Object object(data.packet.id, data.packet.asset_id, data.packet.object_type);
				object.transform = data.packet.transform;
				
				c_world_manager->add_object(object);
				TRACE("Object spawned: ID=" + std::to_string(data.packet.id));
			}
		}
	);
	
	// ObjectDestroy - Object removed from world
	client_object_destroy_sub = ClientEvents::ObjectDestroyEvent::register_callback(
		[this](const ClientEvents::ObjectDestroyEventData& data) {
			if (c_world_manager) {
				c_world_manager->remove_object(data.packet.id);
				TRACE("Object removed: ID=" + std::to_string(data.packet.id));
			}
		}
	);
}
