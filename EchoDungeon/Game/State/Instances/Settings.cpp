#include "Settings.h"
#include "Game/Game.h"
#include "Utils/UI.h"
#include <string>

#include "Utils/SettingsFile.h"

Settings::Settings(Game& game) : GameState(game) {
	// Constructor
}

void Settings::on_activate() {
	TRACE("Settings activated");

	settings_buffer = game.settings; // Load current settings into buffer
}

void Settings::on_deactivate() {
	TRACE("Settings deactivated");
}

void Settings::update() {
	static std::string errors = ""; 
	UIUtils::FullscreenWindow([this]() {
		// Center and enlarge buttons
		ImVec2 button_size(200, 50);
		if (ImGui::Button("Back", button_size)) {
			TRACE("Back button pressed");
			game.state_manager.set_state("MainMenu"); // Switch back to main menu
		}

		// Center title
		ImGui::PushFont(ImGui::GetFont(), 50.0f); // Enlarge font for title
		UIUtils::CentreText("Settings");
		ImGui::PopFont();

		UIUtils::YSpacing(30);

		// Volume label and slider
		UIUtils::CentreText("Volume");
		ImVec2 volume_size(400, 20);
		UIUtils::CentrePosition(volume_size);
		UIUtils::WrapComponent("VolumeSlider", volume_size, [&]() {
			ImGui::SliderInt("", &settings_buffer.volume, 0, 100);
		});

		UIUtils::YSpacing(20);

		// MaxFPS label and input box
		UIUtils::CentreText("Max FPS");
		ImVec2 maxfps_size(150, 20);
		UIUtils::CentrePosition(maxfps_size);
		UIUtils::WrapComponent("MaxFPSInput", maxfps_size, [&]() {
			ImGui::InputInt("", &settings_buffer.max_fps);
		});

		UIUtils::YSpacing(20);

		// Username label and input box
		UIUtils::CentreText("Username");
		ImVec2 username_size(150, 20);
		UIUtils::CentrePosition(username_size);
		UIUtils::WrapComponent("UsernameInput", username_size, [&]() {
			UIUtils::InputText("", &settings_buffer.username);
		});

		UIUtils::YSpacing(30);

		if (!errors.empty()) {
			// Show error popup
			UIUtils::CentreText(errors);
		}

		// Save button
		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Save", button_size)) {
			// Validation
			
			if (settings_buffer.username.empty()) {
				errors = "Username cannot be empty.\n";
				return;
			}
			if (settings_buffer.max_fps < 1) {
				errors = "Max FPS must be greater than 0.\n";
				return;
			}
			if (settings_buffer.volume < 0 || settings_buffer.volume > 100) {
				errors = "Volume must be between 0 and 100.\n";
				return;
			}
			errors.clear();

			TRACE("Settings saved: Volume=" + std::to_string(settings_buffer.volume) +
				  " MaxFPS=" + std::to_string(settings_buffer.max_fps) +
				  " Username=" + settings_buffer.username);

			// Apply settings
			game.settings = settings_buffer;
			game.window.SetTargetFPS(settings_buffer.max_fps); // Set max FPS from settings

			// Save settings
			SettingsFile::save_settings(game.settings);
		}

	});
}