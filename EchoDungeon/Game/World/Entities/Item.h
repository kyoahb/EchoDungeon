#pragma once
#include "Imports/common.h"
#include <cereal/types/string.hpp>

struct ItemEffects {
public:
	int healing = 0; // Adds to health, up to max_health
	float healing_percentage = 0; // Heals a percentage of max health

	int max_health_boost = 0; // Increases max health
	float max_health_percentage_boost = 0; // Increases max health by percentage

	float damage_boost = 0; // Increases damage
	float damage_percentage_boost = 0; // Increases damage by percentage

	float speed_boost = 0; // Increases movement speed
	float speed_percentage_boost = 0; // Increases movement speed by percentage

	float range_boost = 0; // Increases attack range
	float range_percentage_boost = 0; // Increases attack range by percentage

	int atk_cooldown_reduction = 0; // Reduces attack cooldown by flat amount (in milliseconds)
	float atk_cooldown_percent_reduction = 0; // Reduces attack cooldown by percentage

	template <typename Archive>
	void serialize(Archive& archive) {
		archive(healing, healing_percentage, max_health_boost, max_health_percentage_boost,
			damage_boost, damage_percentage_boost, speed_boost, speed_percentage_boost,
			range_boost, range_percentage_boost, atk_cooldown_reduction, atk_cooldown_percent_reduction);
	}
};

class Item {
public:
	
	Item(uint32_t _id,
		const std::string& _asset_id,
		const std::string& _item_name,
		const ItemEffects& _effects):
		id(_id), asset_id(_asset_id), item_name(_item_name),
		effects(_effects) {}

	Item() = default;

	uint32_t id = 0;
	std::string asset_id = "sword";
	std::string item_name = "Default Item";
	ItemEffects effects;
	
	template <typename Archive>
	void serialize(Archive& archive) {
		archive(id, asset_id, item_name, effects);
	}
};