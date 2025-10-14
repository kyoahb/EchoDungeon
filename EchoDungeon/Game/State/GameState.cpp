#include "GameState.h"

GameState::GameState(Game& game) : game(game) {
	// Constructor
}

/**
 * @brief Getter for whether the state is currently active.
 *
 * @returns active True if the state is active, false otherwise
 */
bool GameState::is_active() const {
	return active;
}	

/**
 * @brief Setter for whether the state is currently active. Should only be called by GameStateManager.
 *
 * @param active What the state should be set to
 */
void GameState::set_active(bool active) {
	this->active = active;
}