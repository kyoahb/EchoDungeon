#include "Player.h"
#include "Utils/MathUtils.h"
#include <windows.h>  // For SEH

Player::Player() :
	id(65534),
	is_local(false),
	name("?"),
	transform() {
}

Player::Player(uint32_t _id, bool _is_local, const std::string& _name)
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
	if (is_dead()) {
		std::string dead = "DEAD";
		int health_text_width = MeasureText(dead.c_str(), 18);
		DrawText(dead.c_str(), (int)screen_pos.x - health_text_width / 2, (int)screen_pos.y, 18, RED);
	}
	else {
		std::string health_text = "HP: " + std::to_string(static_cast<int>(health));
		int health_text_width = MeasureText(health_text.c_str(), 18);
		DrawText(health_text.c_str(), (int)screen_pos.x - health_text_width / 2, (int)screen_pos.y, 18, DARKGREEN);
	}

}

void Player::move(const raylib::Vector3& delta) {
	transform.move(delta);
}

void Player::recalculate_stats(const std::unordered_map<uint32_t, Item>& item_registry) {
	// Reset to base stats
	damage = base_damage;
	max_health = base_max_health;
	range = base_range;
	speed = base_speed;
	attack_cooldown = base_attack_cooldown;

	// Apply all items in order
	for (uint32_t item_id : inventory.item_ids) {
		auto it = item_registry.find(item_id);
		if (it != item_registry.end()) {
			const Item& item = it->second;
			apply_item_effects(item.effects);
		}
	}

	// Clamp health to new max_health
	if (health > max_health) {
		health = max_health;
	}
}

void Player::apply_item_effects(const ItemEffects& effects) {
	// Temporary variables to hold modified stats
	auto temp_damage = damage;
	auto temp_max_health = max_health;
	auto temp_range = range;
	auto temp_speed = speed;
	auto temp_attack_cooldown = attack_cooldown;
	auto temp_health = health;

	// Apply flat modifiers first
	temp_damage += effects.damage_boost;
	temp_max_health += effects.max_health_boost;
	temp_range += effects.range_boost;
	temp_speed += effects.speed_boost;
	temp_attack_cooldown -= effects.atk_cooldown_reduction;
	temp_health += effects.healing;

	// Apply percentage modifiers
	temp_damage *= (1.0f + effects.damage_percentage_boost);
	temp_max_health *= (1.0f + effects.max_health_percentage_boost);
	temp_range *= (1.0f + effects.range_percentage_boost);
	temp_speed *= (1.0f + effects.speed_percentage_boost);
	temp_attack_cooldown *= (1.0f - effects.atk_cooldown_percent_reduction);
	temp_health *= (1.0f + effects.healing_percentage);

	temp_health = min(temp_health, temp_max_health); // Clamp health to max_health

	// Ensure no stats are below 0
	damage = max(0, temp_damage);
	max_health = max(1, temp_max_health); // At least 1 max health
	range = max(0.1f, temp_range); // At least 0.1 range
	speed = max(0.1f, temp_speed); // At least 0.1 speed
	attack_cooldown = max(uint64_t(0), temp_attack_cooldown); // At least 0 ms cooldown
	health = max(1, temp_health); // At least 1 health
}

void Player::remove_item_effects(uint32_t item_id, const std::unordered_map<uint32_t, Item>& item_registry) {
	inventory.remove_item(item_id);
	recalculate_stats(item_registry);
}