#include "Enemy.h"
#include "Utils/MathUtils.h"
#include "Player.h"
#include <cfloat>

Enemy::Enemy(uint32_t _id, float _max_health,
	float _speed, float _damage,
	std::string _asset_id)
	: id(_id), max_health(_max_health), health(_max_health),
	speed(_speed), damage(_damage),
	asset_id(_asset_id) {
}

Enemy::Enemy()
	: id(0), max_health(100.0f), health(100.0f),
	speed(2.0f), damage(10.0f),
	asset_id("zombie") {
}

void Enemy::draw3D(const raylib::Camera3D& camera) {

	AxisAngle rot = MathUtils::EulerToAxisAngle(transform.get_rotation());
	AssetMap::get_image_model(asset_id).get().Draw(
		transform.get_position(),
		rot.axis,
		rot.angle,
		transform.get_scale()
	);
}

void Enemy::draw2D(const raylib::Camera3D& camera) {
	// Convert 3D position to 2D screen coordinates
	raylib::Vector3 label_pos = { transform.get_position().x, transform.get_position().y, transform.get_position().z - 1.0f };
	Vector2 screen_pos = GetWorldToScreen(label_pos, camera);

	// Draw health number
	std::string health_text = "HP: " + std::to_string(static_cast<int>(health));
	int health_text_width = MeasureText(health_text.c_str(), 18);
	DrawText(health_text.c_str(), (int)screen_pos.x - health_text_width / 2, (int)screen_pos.y, 18, DARKGREEN);
}

void Enemy::tick(float delta_time, std::vector<Player*>& players) {
	// Simple AI: Move towards the closest player
	if (players.empty()) return;
	Player* closest_player = nullptr;
	float closest_distance = FLT_MAX; // Set it to highest num possible

	for (Player* player : players) {
	float distance = player->transform.get_position().Distance(transform.get_position());
		if (distance < closest_distance) {
			closest_distance = distance;
			closest_player = player;
		}
	}

	if (closest_player) {
		raylib::Vector3 current_pos = transform.get_position();
		raylib::Vector3 target_pos = closest_player->transform.get_position();
		
		// Calculate lerp amount based on speed and delta time
		float lerp_amount = speed * delta_time;
		// Clamp to prevent overshooting
		lerp_amount = min(lerp_amount, 1.0f);
		
		// Lerp towards the target
		raylib::Vector3 new_pos = current_pos.Lerp(target_pos, lerp_amount);
		transform.set_position(new_pos);
	}
}