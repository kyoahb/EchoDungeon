#include "Game/World/Entities/ObjectTransform.h"

ObjectTransform::ObjectTransform()
	: position(0.0f, 0.0f, 0.0f),
	  rotation(0.0f, 0.0f, 0.0f),
	  scale(1.0f, 1.0f, 1.0f),
	  velocity(0.0f, 0.0f, 0.0f),
	  acceleration(0.0f, 0.0f, 0.0f),
	  has_collision(false),
	  is_static(false),
	  has_gravity(false),
	  collision_layer("Default") {
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
void ObjectTransform::move_with_rotation(float forward_delta) {
	// Convert yaw from degrees to radians
	float yaw_radians = rotation.y * (PI / 180.0f);
	// Calculate forward direction vector based on yaw
	raylib::Vector3 forward_direction;
	forward_direction.x = sinf(yaw_radians);
	forward_direction.y = 0.0f; // No vertical movement
	forward_direction.z = -cosf(yaw_radians); // Negative Z is forward in right-handed system
	// Normalize the direction vector
	forward_direction = forward_direction.Normalize();
	// Move position by the forward direction scaled by the delta
	position += forward_direction * forward_delta;
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

// Velocity
void ObjectTransform::set_velocity(const raylib::Vector3& vel) {
	velocity = vel;
}
raylib::Vector3 ObjectTransform::get_velocity() const {
	return velocity;
}

// Acceleration
void ObjectTransform::set_acceleration(const raylib::Vector3& acc) {
	acceleration = acc;
}
raylib::Vector3 ObjectTransform::get_acceleration() const {
	return acceleration;
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

// Gravity
void ObjectTransform::set_has_gravity(bool has_gravity_) {
	has_gravity = has_gravity_;
}
bool ObjectTransform::get_has_gravity() const {
	return has_gravity;
}

// Collision Layer
void ObjectTransform::set_collision_layer(const std::string& layer) {
	collision_layer = layer;
}
std::string ObjectTransform::get_collision_layer() const {
	return collision_layer;
}

// Elasticity
void ObjectTransform::set_elasticity(float elasticity_) {
	elasticity = elasticity_;
}
float ObjectTransform::get_elasticity() const {
	return elasticity;
}