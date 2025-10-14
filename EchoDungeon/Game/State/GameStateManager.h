#pragma once
#include "Imports/common.h"
#include "GameState.h"

class GameStateManager {
public:
	std::unordered_map<std::string, std::shared_ptr<GameState>> states;
	std::shared_ptr<GameState> current_state;

	bool set_state(const std::string& state); // F if could not find/set, T if set
	bool add_state(const std::string& state_name, std::shared_ptr<GameState> state); // F if could not add (name already exists), T if added
	bool free_state(const std::string& state_name); // F if could not find/free, T if freed
	void update();
};