#include "AssetSound.h"

AssetSound::AssetSound(
	const std::string& sound_path
) : sound(raylib::Sound(sound_path)) {
	// Load sound in

	TRACE("AssetSound: Loaded sound from " + sound_path);
}

const raylib::Sound& AssetSound::get() const {
	return sound;
}