#include "Player.h"
#include "Assets/AssetMap.h"

Player::Player(uint16_t _id, bool _is_local, const std::string& _name)
	: id(_id), is_local(_is_local), name(_name) {
}

void Player::draw3D() {
	// For top-down view, draw a cube at player position to represent them
	DrawCube(position, 1.0f, 0.2f, 1.0f, RED);
	DrawCubeWires(position, 1.0f, 0.2f, 1.0f, BLACK);
}

void Player::drawUI(const raylib::Camera3D& camera) {
	// Convert 3D position to 2D screen coordinates
	raylib::Vector3 label_pos = { position.x, position.y, position.z - 1.0f};
	Vector2 screen_pos = GetWorldToScreen(label_pos, camera);
	
	// Draw player name centered above the player
	int text_width = MeasureText(name.c_str(), 20);
	DrawText(name.c_str(), (int)screen_pos.x - text_width / 2, (int)screen_pos.y, 20, BLACK);
}

void Player::move(const raylib::Vector3& delta) {
	position += delta;
}