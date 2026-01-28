#include "Game/World/Entities/ObjectTransform.h"

ObjectTransform::ObjectTransform()
	: position(0.0f, 0.0f, 0.0f),
	  rotation(0.0f, 0.0f, 0.0f),
	  scale(1.0f, 1.0f, 1.0f),
	  has_collision(true),
	  is_static(false){
}

// Position
void ObjectTransform::set_position(const raylib::Vector3& pos) {
	position = pos;
}
raylib::Vector3 ObjectTransform::get_position() const {
	return position;
}
void ObjectTransform::move(const raylib::Vector3& delta) {
	position += delta;
}

// Rotation
void ObjectTransform::set_rotation(const raylib::Vector3& rot) {
	rotation = rot;
}
raylib::Vector3 ObjectTransform::get_rotation() const {
	return rotation;
}
void ObjectTransform::rotate(const raylib::Vector3& delta) {
	rotation += delta;
}

// Scale
void ObjectTransform::set_scale(const raylib::Vector3& scl) {
	scale = scl;
}
raylib::Vector3 ObjectTransform::get_scale() const {
	return scale;
}
void ObjectTransform::scale_by(const raylib::Vector3& factor) {
	scale.x *= factor.x;
	scale.y *= factor.y;
	scale.z *= factor.z;
}
void ObjectTransform::scale_by(float factor) {
	scale.x *= factor;
	scale.y *= factor;
	scale.z *= factor;
}


// Collision
void ObjectTransform::set_has_collision(bool has_collision_) {
	has_collision = has_collision_;
}
bool ObjectTransform::get_has_collision() const {
	return has_collision;
}

// Static
void ObjectTransform::set_is_static(bool is_static_) {
	is_static = is_static_;
}
bool ObjectTransform::get_is_static() const {
	return is_static;
}
