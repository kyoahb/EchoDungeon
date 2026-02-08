#pragma once
#include "Imports/common.h"
#include "Game/GameSettings.h"


class SettingsFile {
public:

	static void save_settings(const GameSettings& settings);
	static GameSettings load_settings();
};