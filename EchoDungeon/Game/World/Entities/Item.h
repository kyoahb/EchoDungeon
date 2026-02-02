#pragma once
#include "Imports/common.h"

struct ItemEffects {
public:
	int healing = 0; // Adds to health, up to max_health
	float healing_percentage = 0; // Heals a percentage of max health

	int max_health_boost = 0; // Increases max health
	float max_health_percentage_boost = 0; // Increases max health by percentage

	int damage_boost = 0; // Increases damage
	float damage_percentage_boost = 0; // Increases damage by percentage

	int speed_boost = 0; // Increases movement speed
	float speed_percentage_boost = 0; // Increases movement speed by percentage

	int range_boost = 0; // Increases attack range
	float range_percentage_boost = 0; // Increases attack range by percentage

	int atk_cooldown_reduction = 0; // Reduces attack cooldown by flat amount (in milliseconds)
	float atk_cooldown_percent_reduction = 0; // Reduces attack cooldown by percentage
};

class Item {
public:
	
	Item(uint32_t _id,
		const std::string& _asset_id,
		const std::string& _item_name,
		const std::string& _item_description,
		const ItemEffects& _effects):
		id(_id), asset_id(_asset_id), item_name(_item_name),
		item_description(_item_description), effects(_effects) {}

	Item() = default;

	uint32_t id = 0;
	std::string asset_id = "sword";
	std::string item_name = "Default Item";
	std::string item_description = "Description";
	ItemEffects effects;
	
};