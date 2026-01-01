#pragma once
#include "Imports/common.h"
#include <unordered_map>
#include <optional>
#include "AssetModel.h"
#include "AssetImage.h"
#include "AssetSound.h"
#include "AssetImageModel.h"  // Added for quad models of images

class AssetMap {
public:
	static std::unordered_map<std::string, AssetModel> models;
	static std::unordered_map<std::string, AssetImage> images;
	static std::unordered_map<std::string, AssetSound> sounds;
	static std::unordered_map<std::string, AssetImageModel> image_models;

	static void load(); // Pre-loads all models
	static const AssetModel& get_model(const std::string& model_id);
	static const AssetImage& get_image(const std::string& image_id);
	static const AssetSound& get_sound(const std::string& sound_id);
	static const AssetImageModel& get_image_model(const std::string& image_id);
};