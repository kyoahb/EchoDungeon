#pragma once
#include "Imports/common.h"
#include "Game/World/Entities/ObjectTransform.h"
#include "Game/World/Assets/AssetMap.h"
class Player;

class Enemy {
public:
	Enemy(uint32_t _id, float _max_health, 
		float _speed, float _damage, 
		std::string _asset_id);
	Enemy();

	uint32_t id = 0;
	float max_health = 100.0f; // Maximum health of the enemy
	float health = 100.0f;  // Current health of the enemy
	float speed = 1.0f; // Movement speed (units per second)
	float damage = 10.0f; // Damage dealt to the player on contact

	ObjectTransform transform;
	std::string asset_id = "zombie"; // By default, a zombie

	void draw3D(const raylib::Camera3D& camera); // Draw the enemy model in 3D space
	void draw2D(const raylib::Camera3D& camera); // Draw 2D UI elements
	void tick(float delta_time, std::vector<Player&>& players); // Update enemy logic per tick

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(id, max_health, health, speed, damage, transform, asset_id);
	}

};
