#pragma once
#include "Imports/common.h"


class AssetSound {
public:
	AssetSound(
		const std::string& sound_path
	);
	raylib::Sound sound;

	const raylib::Sound& get() const;
};
