#pragma once

class Game; // Forward declaration to avoid circular dependency

class GameState {
public:
	GameState(Game& game); // Initialiser

	virtual void on_activate(); // Called when the state is activated
	virtual void on_deactivate(); // Called when the state is deactivated
	virtual void update(); // Called every frame to update the state

	bool is_active() const; // Returns whether the state is currently active
	void set_active(bool active); // Sets whether the state is currently active. Should only be called by GameStateManager
protected:
	bool active = false; // Whether the state is currently active. Set by GameStateManager
	Game& game; // Reference to the main game object. GameState lifetime is shorter than Game lifetime, so this is fine.
};
