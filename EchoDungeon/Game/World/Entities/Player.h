#pragma once
#include "Imports/common.h"

class Player {
public:
	Player(uint16_t _id, bool _is_local, const std::string& _name);
	Player() = default;
	virtual ~Player() = default;

	uint16_t id = 0; // ID linked to network entity
	bool is_local = false; // Is this the local player?
	std::string name = "?"; // Name rendered above the player

	raylib::Vector3 position = { 0.0f, 0.0f, 0.0f }; // 3D position in the world
	raylib::Vector3 rotation = { 0.0f, 0.0f, 0.0f }; // Rotation (Euler angles 0-360) in the world
	float health = 100.0f; // Player health

	void draw3D(); // Draw the player model in 3D space
	void drawUI(const raylib::Camera3D& camera); // Draw 2D UI elements (name, health) using screen coordinates
	void move(const raylib::Vector3& delta); // Move the player by delta

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(id, is_local, name, position.x, position.y, position.z,
			rotation.x, rotation.y, rotation.z, health);
	}		

private:
	// Setup player assets later


};
