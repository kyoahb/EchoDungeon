#include "UI.h"
#include <imgui.h>
#include <raylib.h>  // For GetScreenWidth, GetScreenHeight

void UIUtils::FullscreenWindow(std::function<void()> content) {
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ (float)GetScreenWidth(), (float)GetScreenHeight() });

	// Simple main menu UI
	ImGui::Begin("FullscreenWindow", NULL,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoBringToFrontOnFocus
	);

	content(); // Render whatever content inside the window

	ImGui::End();
}

void UIUtils::CentreText(const std::string& text) {
	ImVec2 textSize = ImGui::CalcTextSize(text.c_str()); // Calculate size of text, were it to be drawn 
	float windowWidth = ImGui::GetWindowWidth(); // Calculate size of window
	ImGui::SetCursorPosX((windowWidth - textSize.x) / 2); // Set next draw size = centre - componentSize/2
	ImGui::Text(text.c_str()); // Draw text

}

void UIUtils::CentrePosition(ImVec2 component_size) {
	float windowWidth = ImGui::GetWindowWidth(); // Calculate size of window
	ImGui::SetCursorPosX((windowWidth - component_size.x) / 2); // Set next draw size = centre - componentSize/2
}