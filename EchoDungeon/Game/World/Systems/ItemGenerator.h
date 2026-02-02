#pragma once
#include "Imports/common.h"
#include "Game/World/Entities/Item.h"
#include <random>

/**
 * @brief Static class for generating random items with stats that scale with game difficulty.
 * Items become more significant as game time progresses, with larger stat changes and more effects.
 * Early game: fewer stat changes, smaller ranges. Late game: more stat changes, larger ranges.
 */
class ItemGenerator {
public:
	/**
	 * @brief Generate a random item with stats scaled by game time (difficulty).
	 * @param item_id The unique ID for this item
	 * @param game_time_ms Milliseconds since game start (used for difficulty scaling)
	 * @return A randomly generated Item with effects scaled by difficulty
	 */
	static Item generate_random_item(uint32_t item_id, uint64_t game_time_ms);

private:
	static std::mt19937 rng;
	
	// Helper functions
	static float get_difficulty_multiplier(uint64_t game_time_ms);
	static std::string generate_item_name(const ItemEffects& effects, std::vector<float> dist_rolls, float difficulty);
	static std::string choose_asset_id(const ItemEffects& effects);
	static int count_negative_stats(const ItemEffects& effects);
	static int count_positive_stats(const ItemEffects& effects);
};
