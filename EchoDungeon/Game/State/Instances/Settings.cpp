#include "Settings.h"
#include "Game/Game.h"
#include "Utils/UI.h"
#include <string>

Settings::Settings(Game& game) : GameState(game) {
	// Constructor
}

void Settings::on_activate() {
	TRACE("Settings activated");
}

void Settings::on_deactivate() {
	TRACE("Settings deactivated");
}

void Settings::update() {
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
		static int volume = 50;
		UIUtils::CentrePosition(volume_size);
		UIUtils::WrapComponent("VolumeSlider", volume_size, [&]() {
			ImGui::SliderInt("", &volume, 0, 100);
		});

		UIUtils::YSpacing(20);

		// MaxFPS label and input box
		UIUtils::CentreText("Max FPS");
		ImVec2 maxfps_size(150, 20);
		static int maxfps = 120;
		UIUtils::CentrePosition(maxfps_size);
		UIUtils::WrapComponent("MaxFPSInput", maxfps_size, [&]() {
			ImGui::InputInt("", &maxfps);
		});

		UIUtils::YSpacing(20);

		// Username label and input box
		UIUtils::CentreText("Username");
		ImVec2 username_size(150, 20);
		static std::string username = "John";
		UIUtils::CentrePosition(username_size);
		UIUtils::WrapComponent("UsernameInput", username_size, [&]() {
			UIUtils::InputText("", &username);
		});

		UIUtils::YSpacing(30);

		// Save button
		UIUtils::CentrePosition(button_size);
		if (ImGui::Button("Save", button_size)) {
			TRACE("Save button pressed");
			
			// Print out all information as a test
			TRACE("Settings saved:");
			TRACE(" - Volume: " + std::to_string(volume));
			TRACE(" - Max FPS: " + std::to_string(maxfps));
			TRACE(" - Username: " + username);
		}

	});
}