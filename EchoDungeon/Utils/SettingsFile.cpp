#include "SettingsFile.h"
#include <cereal/archives/json.hpp>
#include <cereal/types/string.hpp>
#include <fstream>
#include <filesystem>

/*
* @brief Handles saving to a file
* 
* @param settings The GameSettings struct to save
*/
void SettingsFile::save_settings(const GameSettings& settings) {
	try {
		// Ensure Assets directory exists
		std::filesystem::create_directories("Assets");

		// Open the file for writing
		std::ofstream os("Assets/settings.json");
		if (!os.is_open()) {
			ERROR("Failed to open Assets/settings.json for writing");
			return;
		}

		// Serialize settings to JSON
		cereal::JSONOutputArchive archive(os);
		archive(cereal::make_nvp("settings", settings));

		INFO("Settings saved successfully to Assets/settings.json");
	} catch (const std::exception& e) {
		ERROR("Failed to save settings: " + std::string(e.what()));
	}
}

/*
* @brief Handles loading settings from a file
*
* @return The loaded GameSettings struct, or default values if loading fails
*/
GameSettings SettingsFile::load_settings() {
	GameSettings settings; // Start with default values

	try {
		// Check if the file exists
		if (!std::filesystem::exists("Assets/settings.json")) {
			INFO("Assets/settings.json not found, using default settings");
			return settings;
		}

		// Open the file for reading
		std::ifstream is("Assets/settings.json");
		if (!is.is_open()) {
			ERROR("Failed to open Assets/settings.json for reading, using default settings");
			return settings;
		}

		// Deserialize settings from JSON
		cereal::JSONInputArchive archive(is);
		archive(cereal::make_nvp("settings", settings));

		INFO("Settings loaded successfully from Assets/settings.json");
	} catch (const std::exception& e) {
		ERROR("Failed to load settings: " + std::string(e.what()) + ", using default settings");
	}

	return settings;
}