#pragma once
#include "Imports/common.h"
#include "Game/World/Entities/ObjectTransform.h"
#include "Game/World/Assets/AssetMap.h"
class Player {
public:
	Player(uint32_t _id, bool _is_local, const std::string& _name);
	Player();
	virtual ~Player() {}

	uint32_t id = 0; // ID linked to network entity
	bool is_local = false; // Is this the local player?
	std::string name = "?"; // Name rendered above the player

	ObjectTransform transform; // Player transform (position, rotation, scale)

	float health = 100.0f; // Player health
	float damage = 10.0f; // Damage dealt by the player
	float max_health = 100.0f; // Maximum health
	float range = 2.0f; // Attack range
	float speed = 2.0f; // Units per second 

	std::string asset_id = "player";

	void draw3D(const raylib::Camera3D& camera); // Draw the player model in 3D space
	void draw2D(const raylib::Camera3D& camera); // Draw 2D UI elements (name, health) using screen coordinates
	void move(const raylib::Vector3& delta); // Move the player by delta

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(id, is_local, name, 
			transform, health, asset_id,
			damage, max_health, range, speed);
	}		
};
