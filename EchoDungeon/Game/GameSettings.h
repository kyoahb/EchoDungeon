#pragma once
#include "Imports/common.h"

struct GameSettings {
	int volume = 50;				 // Volume level (0-100)
	int max_fps = 120;				 // Maximum frames per second
	std::string username = "Player"; // Player's username
};