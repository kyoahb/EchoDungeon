#include "GameStateManager.h"

/*
 * @brief Adds a state to the state map, so it can be activated later.
 *
 * @param state_name The name to associate with the state. This is the key
 * @param state A shared ptr to an instance of the state to add
 * 
 * @returns bool True if the state was added successfully, false if a state with this name already exists
 */
bool GameStateManager::add_state(const std::string& state_name, std::shared_ptr<GameState> state) {
	if (states.find(state_name) != states.end()) {
		WARNING("State with name '" + state_name + "' already exists. Could not add state.");
		return false; // State with this name already exists
	}
	states[state_name] = state;
	TRACE("State with name '" + state_name + "' added successfully.");
	return true;
}

/**
 * @brief Activate a state that is stored in the state map.
 *
 * @param state_name The name of the state to activate
 * 
 * @returns bool True if the state was found and activated successfully, false otherwise
 */
bool GameStateManager::set_state(const std::string& state_name) {
	auto it = states.find(state_name);
	if (it == states.end()) {
		WARNING("State with name '" + state_name + "' not found. Could not set state.");
		return false; // State with this name does not exist
	}
	// If we have a current state, deactivate it
	if (current_state) {
		current_state->on_deactivate();
		current_state->set_active(false);
		//TRACE("State deactivated.");
	}
	// Set the new current state and activate it
	current_state = it->second;
	current_state->set_active(true);
	current_state->on_activate();
	//TRACE("State with name '" + state_name + "' activated successfully.");
	return true;
}

/**
 * @brief Calls update on the current active state, if there is one.
 */
void GameStateManager::update() {
	if (current_state && current_state->is_active()) {
		current_state->update();
	}
}