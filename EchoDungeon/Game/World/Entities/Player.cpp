#include "Player.h"
#include "Utils/MathUtils.h"

Player::Player() :
	id(65534),
	is_local(false),
	name("?"),
	transform() {
}

Player::Player(uint16_t _id, bool _is_local, const std::string& _name)
	: id(_id), is_local(_is_local), name(_name), transform(){ 
}

void Player::draw3D(const raylib::Camera3D& camera) {
	// Get axis angle rotation
	AxisAngle rot = MathUtils::EulerToAxisAngle(transform.get_rotation());

	AssetMap::get_image_model("player").get().Draw(
		transform.get_position(),
		rot.axis,
		rot.angle,
		transform.get_scale()
		);
}

void Player::draw2D(const raylib::Camera3D& camera) {
	// Convert 3D position to 2D screen coordinates
	raylib::Vector3 label_pos = { transform.get_position().x, transform.get_position().y, transform.get_position().z - 1.0f};
	Vector2 screen_pos = GetWorldToScreen(label_pos, camera);

	// Draw player name centered above the player
	int text_width = MeasureText(name.c_str(), 20);
	DrawText(name.c_str(), (int)screen_pos.x - text_width / 2, (int)screen_pos.y - 20, 20, BLACK);

	// Draw health number below the name
	std::string health_text = "HP: " + std::to_string(static_cast<int>(health));
	int health_text_width = MeasureText(health_text.c_str(), 18);
	DrawText(health_text.c_str(), (int)screen_pos.x - health_text_width / 2, (int)screen_pos.y, 18, DARKGREEN);
}

void Player::move(const raylib::Vector3& delta) {
	transform.move(delta);
}