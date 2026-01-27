#include "PhysicsManager.h"

void PhysicsManager::update(
	std::unordered_map<uint16_t, Player>* players, 
	std::unordered_map<uint16_t, Object>* objects) {
	
	if (!players || !objects) return;

	// Pre-calculate all object bounding boxes once per frame
	std::unordered_map<uint16_t, raylib::BoundingBox> object_boxes;
	object_boxes.reserve(objects->size());

	for (auto& [object_id, object] : *objects) {
		ObjectTransform& object_transform = object.transform;
		
		// Skip if object has no collision
		if (!object_transform.get_has_collision()) continue;

		raylib::Vector3 object_pos = object_transform.get_position();
		raylib::Vector3 object_scale = object_transform.get_scale();

		// Create and cache bounding box for this object
		object_boxes[object_id] = raylib::BoundingBox(
			raylib::Vector3(
				object_pos.x - object_scale.x * 0.5f,
				object_pos.y - object_scale.y * 0.5f,
				object_pos.z - object_scale.z * 0.5f
			),
			raylib::Vector3(
				object_pos.x + object_scale.x * 0.5f,
				object_pos.y + object_scale.y * 0.5f,
				object_pos.z + object_scale.z * 0.5f
			)
		);
	}

	// Check each player against all cached object bounding boxes
	for (auto& [player_id, player] : *players) {
		ObjectTransform& player_transform = player.transform;
		
		// Skip if player has no collision
		if (!player_transform.get_has_collision()) continue;

		raylib::Vector3 player_pos = player_transform.get_position();
		raylib::Vector3 player_scale = player_transform.get_scale();

		// Create bounding box for player (assuming centered origin)
		raylib::BoundingBox player_box(
			raylib::Vector3(
				player_pos.x - player_scale.x * 0.5f,
				player_pos.y - player_scale.y * 0.5f,
				player_pos.z - player_scale.z * 0.5f
			),
			raylib::Vector3(
				player_pos.x + player_scale.x * 0.5f,
				player_pos.y + player_scale.y * 0.5f,
				player_pos.z + player_scale.z * 0.5f
			)
		);

		// Check against all cached object bounding boxes
		for (auto& [object_id, object_box] : object_boxes) {
			// Get the actual object for position data (needed for pushback calculation)
			auto obj_it = objects->find(object_id);
			if (obj_it == objects->end()) continue;
			
			Object& object = obj_it->second;
			raylib::Vector3 object_pos = object.transform.get_position();

			// Check collision
			if (player_box.CheckCollision(object_box)) {
				// Calculate penetration depth on each axis
				float penetration_x = (std::min)(
					player_box.max.x - object_box.min.x,
					object_box.max.x - player_box.min.x
				);
				float penetration_y = (std::min)(
					player_box.max.y - object_box.min.y,
					object_box.max.y - player_box.min.y
				);
				float penetration_z = (std::min)(
					player_box.max.z - object_box.min.z,
					object_box.max.z - player_box.min.z
				);

				// Find the axis with minimum penetration (this is the collision normal axis)
				raylib::Vector3 pushback(0.0f, 0.0f, 0.0f);
				
				if (penetration_x <= penetration_y && penetration_x <= penetration_z) {
					// Push along X axis
					pushback.x = (player_pos.x < object_pos.x) ? -penetration_x : penetration_x;
				}
				else if (penetration_y <= penetration_x && penetration_y <= penetration_z) {
					// Push along Y axis
					pushback.y = (player_pos.y < object_pos.y) ? -penetration_y : penetration_y;
				}
				else {
					// Push along Z axis
					pushback.z = (player_pos.z < object_pos.z) ? -penetration_z : penetration_z;
				}

				// Apply pushback to player
				player_transform.move(pushback);
			}
		}
	}
}
