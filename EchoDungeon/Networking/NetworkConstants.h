#pragma once
#include "Imports/common.h"

namespace NetworkConstants {
	// Sleep duration for the networking loop in milliseconds (Server/Client)
	constexpr const long LOOP_SLEEP_DURATION_MS = 1;

	// Max simultaneous connections (Server)
	constexpr const int MAX_SIMULTANEOUS_CONNECTIONS = 4;

	// Default port for networking (Server/Client)
	constexpr const uint16_t DEFAULT_PORT = 7422;

	// Maximum number of channels for ENet communication (Server/Client)
	constexpr const int MAX_CHANNELS = 2;

	// Bandwidth limits for ENet (Server/Client)
	constexpr const int BANDWIDTH_LIMIT = 0;   // Unlimited

	// Timeout duration for built-in ENet (Server/Client)
	constexpr const int ENET_TIMEOUT = 0; // No timeout

}