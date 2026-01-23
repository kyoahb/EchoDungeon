#pragma once

#include "Imports/common.h"

class ObjectTransform {

private:
	raylib::Vector3 position;
	raylib::Vector3 rotation; // Euler angles in degrees 0-360
	raylib::Vector3 scale;
	bool has_collision = false;
	bool is_static = false;

public:
	ObjectTransform();

	// Position
	void set_position(const raylib::Vector3& pos);
	raylib::Vector3 get_position() const;
	void move(const raylib::Vector3& delta);

	// Rotation
	void set_rotation(const raylib::Vector3& rot);
	raylib::Vector3 get_rotation() const;
	void rotate(const raylib::Vector3& delta);

	// Scale
	void set_scale(const raylib::Vector3& scl);
	raylib::Vector3 get_scale() const;
	void scale_by(const raylib::Vector3& factor);
	void scale_by(float factor);


	// Collision
	void set_has_collision(bool has_collision);
	bool get_has_collision() const;

	// Static
	void set_is_static(bool is_static);
	bool get_is_static() const;


	template <class Archive>
	void serialize(Archive& archive) {
		archive(position, rotation, scale, has_collision, is_static);
	}



};