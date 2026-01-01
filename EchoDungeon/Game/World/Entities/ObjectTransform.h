#pragma once

#include "Imports/common.h"

class ObjectTransform {

private:
	raylib::Vector3 position;
	raylib::Vector3 rotation; // Euler angles in degrees 0-360
	raylib::Vector3 scale;
	raylib::Vector3 velocity;
	raylib::Vector3 acceleration;
	bool has_collision = false;
	bool is_static = false;
	bool has_gravity = false;
	std::string collision_layer = "Default";
	float elasticity = 0.0f; // inelastic collision by default

public:
	ObjectTransform();

	// Position
	void set_position(const raylib::Vector3& pos);
	raylib::Vector3 get_position() const;
	void move(const raylib::Vector3& delta);
	void move_with_rotation(float forward_delta); // Moves forward/backward based on current rotation

	// Rotation
	void set_rotation(const raylib::Vector3& rot);
	raylib::Vector3 get_rotation() const;
	void rotate(const raylib::Vector3& delta);

	// Scale
	void set_scale(const raylib::Vector3& scl);
	raylib::Vector3 get_scale() const;
	void scale_by(const raylib::Vector3& factor);
	void scale_by(float factor);

	// Velocity
	void set_velocity(const raylib::Vector3& vel);
	raylib::Vector3 get_velocity() const;
	
	// Acceleration
	void set_acceleration(const raylib::Vector3& acc);
	raylib::Vector3 get_acceleration() const;

	// Collision
	void set_has_collision(bool has_collision);
	bool get_has_collision() const;

	// Static
	void set_is_static(bool is_static);
	bool get_is_static() const;

	// Gravity
	void set_has_gravity(bool has_gravity);
	bool get_has_gravity() const;

	// Collision Layer
	void set_collision_layer(const std::string& layer);
	std::string get_collision_layer() const;

	// Elasticity
	void set_elasticity(float elasticity);
	float get_elasticity() const;

	template <class Archive>
	void serialize(Archive& archive) {
		archive(position, rotation, scale, velocity, acceleration, has_collision, is_static, has_gravity, collision_layer, elasticity);
	}



};