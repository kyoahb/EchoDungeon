#include "AssetMap.h"

std::unordered_map<std::string, AssetModel> AssetMap::models = {};
std::unordered_map<std::string, AssetImage> AssetMap::images = {};
std::unordered_map<std::string, AssetSound> AssetMap::sounds = {};
std::unordered_map<std::string, AssetImageModel> AssetMap::image_models = {};  // Added

const AssetModel& AssetMap::get_model(const std::string& model_id) {
	auto it = models.find(model_id);
	if (it != models.end()) {
		return it->second;
	} else {
		ERROR("MODEL ID '" + model_id + "' not found in AssetMap.");
		throw;
	}
}

const AssetImage& AssetMap::get_image(const std::string& image_id) {
	auto it = images.find(image_id);
	if (it != images.end()) {
		return it->second;
	}
	else {
		ERROR("IMAGE ID '" + image_id + "' not found in AssetMap.");
		throw;
	}
}

const AssetSound& AssetMap::get_sound(const std::string& sound_id) {
	auto it = sounds.find(sound_id);
	if (it != sounds.end()) {
		return it->second;
	}
	else {
		ERROR("SOUND ID '" + sound_id + "' not found in AssetMap.");
		throw;
	}
}

const AssetImageModel& AssetMap::get_image_model(const std::string& image_id) {  // Added
	auto it = image_models.find(image_id);
	if (it != image_models.end()) {
		return it->second;  // Return a copy for per-instance use
	} else {
		ERROR("IMAGE MODEL ID '" + image_id + "' not found in AssetMap.");
		throw;
	}
}

void AssetMap::load() {
	// Add all models to be loaded here
	models.insert({
		"cube",
		AssetModel("Assets/models/cube.glb")
		});

	// Add all images to be loaded here
	images.insert({
		"player",
		AssetImage("Assets/images/player.png")
		});

	// Add all sounds to be loaded here

	// Generate quad models for images
	for (auto& pair : images) {
		image_models.emplace(pair.first, AssetImageModel(pair.second));
	}

	TRACE("AssetMap: Loaded " + std::to_string(models.size()) + " models.");
	TRACE("AssetMap: Loaded " + std::to_string(images.size()) + " images.");
	TRACE("AssetMap: Loaded " + std::to_string(sounds.size()) + " sounds.");
}