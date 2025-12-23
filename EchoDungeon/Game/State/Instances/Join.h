#pragma once
#include "Game/State/GameState.h"
#include <future>
#include <memory>
#include "Networking/Client/Client.h"

class Join : public GameState {
public:
	Join(Game& game); // Initialiser
	void on_activate() override; // Called when the state is activated
	void on_deactivate() override; // Called when the state is deactivated
	void update() override; // Called every frame to update the state

private:
	enum class ConnectionStatus {
		IDLE,           // Not connecting
		CONNECTING,     // Connection in progress
		CONNECTED,      // Successfully connected
		FAILED          // Connection failed
	};

	ConnectionStatus connection_status = ConnectionStatus::IDLE;
	std::shared_ptr<std::future<ConnectionResult>> connection_future;
	std::string connection_error_message;
};