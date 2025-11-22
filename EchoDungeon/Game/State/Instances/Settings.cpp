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

		// Volume label and slider
		UIUtils::CentreText("Volume");
		ImVec2 volume_size(400, 20);
		static int volume = 50; // Static to ensure that the 'volume' variable is not re-initialised every frame
		UIUtils::CentrePosition(volume_size); // Centre volume slider
		ImGui::PushItemWidth(volume_size.x); // Ensure volume slider has a fixed width
		ImGui::PushID("VolumeSlider"); // Use an ID (necessary for ImGui for an item without label text to have its own ID)
		ImGui::SliderInt("", &volume, 0, 100); // Draw a volume bar, dont use any side-label ("" = blank text)
		ImGui::PopID(); // Remove ID from other components
		ImGui::PopItemWidth(); // Stop using that width

		// MaxFPS label and input box
		UIUtils::CentreText("Max FPS");
		ImVec2 maxfps_size(150, 20);
		static int maxfps = 120; // Static to ensure that the 'maxfps' variable is not re-initialised every frame
		UIUtils::CentrePosition(maxfps_size); // Centre maxfps input box
		ImGui::PushItemWidth(maxfps_size.x); // Ensure maxfps input box has a fixed width
		ImGui::PushID("MaxFPSInput");
		ImGui::InputInt("", &maxfps);
		ImGui::PopID();
		ImGui::PopItemWidth();

		// Username label and input box
		UIUtils::CentreText("Username");
		ImVec2 username_size(150, 20);
		static std::string username = "John";
		UIUtils::CentrePosition(username_size);
		ImGui::PushItemWidth(username_size.x);
		ImGui::PushID("UsernameInput");
		UIUtils::InputText("", &username);
		ImGui::PopID();
		ImGui::PopItemWidth();

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