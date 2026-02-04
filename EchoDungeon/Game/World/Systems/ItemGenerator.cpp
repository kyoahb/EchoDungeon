#include "ItemGenerator.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <numeric>
// Initialize random number generator with current time
std::mt19937 ItemGenerator::rng(std::chrono::steady_clock::now().time_since_epoch().count());

// Healing
const static int MIN_HEALING = -10;
const static int MAX_HEALING = 10;
const static float MIN_HEALING_PERCENT = -0.4f;
const static float MAX_HEALING_PERCENT = 0.4f;

// Max Health
const static int MIN_MAX_HEALTH_BOOST = -5;
const static int MAX_MAX_HEALTH_BOOST = 5;
const static float MIN_MAX_HEALTH_PERCENT_BOOST = -0.1f;
const static float MAX_MAX_HEALTH_PERCENT_BOOST = 0.1f;

// Damage
const static int MIN_DAMAGE_BOOST = -2;
const static int MAX_DAMAGE_BOOST = 2;
const static float MIN_DAMAGE_PERCENT_BOOST = -0.2f;
const static float MAX_DAMAGE_PERCENT_BOOST = 0.2f;

// Speed
const static float MIN_SPEED_BOOST = -1.0f;
const static float MAX_SPEED_BOOST = 1.0f;
const static float MIN_SPEED_PERCENT_BOOST = -0.1f;
const static float MAX_SPEED_PERCENT_BOOST = 0.1f;

// Range
const static float MIN_RANGE_BOOST = -1.0f;
const static float MAX_RANGE_BOOST = 1.0f;
const static float MIN_RANGE_PERCENT_BOOST = -0.1f;
const static float MAX_RANGE_PERCENT_BOOST = 0.1f;

// Attack Cooldown
const static int MIN_ATK_COOLDOWN_REDUCTION = -10;
const static int MAX_ATK_COOLDOWN_REDUCTION = 10;
const static float MIN_ATK_COOLDOWN_PERCENT_REDUCTION = -0.2;
const static float MAX_ATK_COOLDOWN_PERCENT_REDUCTION = 0.2f;

// Enum to represent all possible stats
enum class StatType {
	MaxHealthBoost,
	MaxHealthPercentBoost,
	DamageBoost,
	DamagePercentBoost,
	SpeedBoost,
	SpeedPercentBoost,
	RangeBoost,
	RangePercentBoost,
	AtkCooldownReduction,
	AtkCooldownPercentReduction,
	Healing,
	HealingPercent,
	COUNT // Total number of stat types
};

Item ItemGenerator::generate_random_item(uint32_t item_id, uint64_t game_time_ms) {
	// Calculate difficulty multiplier based on game time
	float difficulty = get_difficulty_multiplier(game_time_ms);
	
	ItemEffects effects;
	
	// Determine how many stats this item will have
	// Range: 1 to total number of possible stats
	std::binomial_distribution<int> num_stats_dist(
		static_cast<int>(StatType::COUNT)-1, (1- 0.8f*difficulty));
	int num_stats = 1 + num_stats_dist(rng);
	
	// Create a list of all possible stats and shuffle it
	std::vector<StatType> available_stats;
	for (int i = 0; i < static_cast<int>(StatType::COUNT); ++i) {
		available_stats.push_back(static_cast<StatType>(i));
	}
	std::shuffle(available_stats.begin(), available_stats.end(), rng);
	
	// List of all rolls to calculate how lucky this item is
	std::vector<float> dist_rolls = {};

	// Helper to generate a random value in a range, scaled by difficulty
	auto generate_value = [&](float min, float max) -> float {
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		float range_min = min * difficulty;
		float range_max = max * difficulty;
		float roll = dist(rng);
		dist_rolls.push_back(roll); // Add to roll list to calculate quality
		return range_min + roll * (range_max - range_min);
	};
	
	// Apply the first 'num_stats' stats from the shuffled list
	for (int i = 0; i < num_stats; ++i) {
		StatType stat = available_stats[i];
		
		switch (stat) {
			case StatType::MaxHealthBoost:
				effects.max_health_boost = static_cast<int>(
					generate_value(MIN_MAX_HEALTH_BOOST, MAX_MAX_HEALTH_BOOST));
				break;
				
			case StatType::MaxHealthPercentBoost:
				effects.max_health_percentage_boost = 
					generate_value(MIN_MAX_HEALTH_PERCENT_BOOST, MAX_MAX_HEALTH_PERCENT_BOOST);
				break;
				
			case StatType::DamageBoost:
				effects.damage_boost = generate_value(MIN_DAMAGE_BOOST, MAX_DAMAGE_BOOST);
				break;
				
			case StatType::DamagePercentBoost:
				effects.damage_percentage_boost = 
					generate_value(MIN_DAMAGE_PERCENT_BOOST, MAX_DAMAGE_PERCENT_BOOST);
				break;
				
			case StatType::SpeedBoost:
				effects.speed_boost = 
					generate_value(MIN_SPEED_BOOST, MAX_SPEED_BOOST);
				break;
				
			case StatType::SpeedPercentBoost:
				effects.speed_percentage_boost = 
					generate_value(MIN_SPEED_PERCENT_BOOST, MAX_SPEED_PERCENT_BOOST);
				break;
				
			case StatType::RangeBoost:
				effects.range_boost = 
					generate_value(MIN_RANGE_BOOST, MAX_RANGE_BOOST);
				break;
				
			case StatType::RangePercentBoost:
				effects.range_percentage_boost = 
					generate_value(MIN_RANGE_PERCENT_BOOST, MAX_RANGE_PERCENT_BOOST);
				break;
				
			case StatType::AtkCooldownReduction:
				effects.atk_cooldown_reduction = static_cast<int>(
					generate_value(MIN_ATK_COOLDOWN_REDUCTION, MAX_ATK_COOLDOWN_REDUCTION));
				break;
				
			case StatType::AtkCooldownPercentReduction:
				effects.atk_cooldown_percent_reduction = 
					generate_value(MIN_ATK_COOLDOWN_PERCENT_REDUCTION, MAX_ATK_COOLDOWN_PERCENT_REDUCTION);
				break;
				
			case StatType::Healing:
				effects.healing = static_cast<int>(
					generate_value(MIN_HEALING, MAX_HEALING));
				break;
				
			case StatType::HealingPercent:
				effects.healing_percentage = 
					generate_value(MIN_HEALING_PERCENT, MAX_HEALING_PERCENT);
				break;
				
			case StatType::COUNT:
				break;
		}
	}
	
	// Choose asset based on damage vs armor focus
	std::string asset_id = choose_asset_id(effects);
	
	// Generate name based on quality and danger level
	std::string name = generate_item_name(effects, dist_rolls, difficulty);
	
	INFO("Generated item " + std::to_string(item_id) + " (" + name + ") with " + 
	     std::to_string(num_stats) + " stats at difficulty " + std::to_string(difficulty));
	
	return Item(item_id, asset_id, name, effects);
}

float ItemGenerator::get_difficulty_multiplier(uint64_t game_time_ms) {
	// Difficulty scales with time: 1.0x to 5.0x
	float seconds = game_time_ms / 1000.0f;
	
	// Exponential decay curve
	float multiplier = 1.0f + 5.0f * (1 - std::exp(-(1 / 1500) * seconds));
	// 1.0x at 0s
	// ~2.6x at 600s (10 min)
	// ~3.8x at 1200s (20 min)
	// ~4.5x at 1800s (30 min)

	// Cap at 10.0x
	return min(multiplier, 5.0f);
}

std::string ItemGenerator::generate_item_name(const ItemEffects& effects, std::vector<float> dist_rolls, float difficulty) {
	float goodness = (std::accumulate(dist_rolls.begin(), dist_rolls.end(), 0.0f) / dist_rolls.size())
		* std::sqrt((int)dist_rolls.size() / 12.0f);
	// 'Goodness' is a measure of how lucky the item is.
	// The luckiest item has all rolls at 1.0 and has 12 stats, giving a goodness of 1.0.
	// The least lucky item has all rolls at 0.0 and has 1 stat, giving a goodness of 0.0.
	// This is used to determine the 'rarity' of the item in the name.

	// Determine rarity prefix based on goodness
	std::string rarity_prefix;
	if (goodness >= 0.95f) {
		rarity_prefix = "Legendary";
	}
	else if (goodness >= 0.8f) {
		rarity_prefix = "Epic";
	}
	else if (goodness >= 0.5f) {
		rarity_prefix = "Rare";
	}
	else {
		rarity_prefix = "Common";
	}

	// Determine danger prefix based on negative stats
	int negative_count = count_negative_stats(effects);
	int positive_count = count_positive_stats(effects);
	int total_count = negative_count + positive_count;

	std::string danger_prefix = "";
	if (total_count > 0) {
		if (negative_count > total_count / 2) {
			danger_prefix = "Cursed ";
		}
		else if (negative_count > 0) {
			danger_prefix = "Dangerous ";
		}
	}

	return danger_prefix + rarity_prefix + " Item";
}

std::string ItemGenerator::choose_asset_id(const ItemEffects& effects) {
	// Count damage-related stats vs armor-related stats
	int damage_stats = 0;
	int armor_stats = 0;
	
	// Damage-related: damage_boost, damage_percentage_boost, range_boost, range_percentage_boost, 
	//                 atk_cooldown_reduction, atk_cooldown_percent_reduction
	if (effects.damage_boost != 0) damage_stats++;
	if (effects.damage_percentage_boost != 0) damage_stats++;
	if (effects.range_boost != 0) damage_stats++;
	if (effects.range_percentage_boost != 0) damage_stats++;
	if (effects.atk_cooldown_reduction != 0) damage_stats++;
	if (effects.atk_cooldown_percent_reduction != 0) damage_stats++;
	
	// Armor-related: max_health_boost, max_health_percentage_boost, speed_boost, 
	//                speed_percentage_boost, healing, healing_percentage
	if (effects.max_health_boost != 0) armor_stats++;
	if (effects.max_health_percentage_boost != 0) armor_stats++;
	if (effects.speed_boost != 0) armor_stats++;
	if (effects.speed_percentage_boost != 0) armor_stats++;
	if (effects.healing != 0) armor_stats++;
	if (effects.healing_percentage != 0) armor_stats++;
	
	return damage_stats > armor_stats ? "sword" : "armour";
}

int ItemGenerator::count_negative_stats(const ItemEffects& effects) {
	int count = 0;
	if (effects.max_health_boost < 0) count++;
	if (effects.damage_boost < 0) count++;
	if (effects.speed_boost < 0) count++;
	if (effects.range_boost < 0) count++;
	if (effects.atk_cooldown_reduction < 0) count++;
	if (effects.healing < 0) count++;
	if (effects.max_health_percentage_boost < 0) count++;
	if (effects.damage_percentage_boost < 0) count++;
	if (effects.speed_percentage_boost < 0) count++;
	if (effects.range_percentage_boost < 0) count++;
	if (effects.atk_cooldown_percent_reduction < 0) count++;
	if (effects.healing_percentage < 0) count++;
	return count;
}

int ItemGenerator::count_positive_stats(const ItemEffects& effects) {
	int count = 0;
	if (effects.max_health_boost > 0) count++;
	if (effects.damage_boost > 0) count++;
	if (effects.speed_boost > 0) count++;
	if (effects.range_boost > 0) count++;
	if (effects.atk_cooldown_reduction > 0) count++;
	if (effects.healing > 0) count++;
	if (effects.max_health_percentage_boost > 0) count++;
	if (effects.damage_percentage_boost > 0) count++;
	if (effects.speed_percentage_boost > 0) count++;
	if (effects.range_percentage_boost > 0) count++;
	if (effects.atk_cooldown_percent_reduction > 0) count++;
	if (effects.healing_percentage > 0) count++;
	return count;
}
