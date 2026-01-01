#pragma once
#include "Imports/common.h"
#include "GameState.h"
#include "Game/Events/EventList.h"

class GameStateManager {
public:

	GameStateManager();
	~GameStateManager();

	std::unordered_map<std::string, std::shared_ptr<GameState>> states;
	std::shared_ptr<GameState> current_state;

	bool set_state(const std::string& state); // F if could not find/set, T if set
	bool add_state(const std::string& state_name, std::shared_ptr<GameState> state); // F if could not add (name already exists), T if added
	void update();

	// callback handlers
	void handle_state_change(const ClientEvents::StateChangeEventData& data);
private:
	// Callbacks for events
	int on_state_change_callback = -1; // Server requested a state change
};