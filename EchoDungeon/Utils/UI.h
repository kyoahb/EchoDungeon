#pragma once

#include <functional>
#include <string>
#include <imgui.h>

class UIUtils {
public:
	static void FullscreenWindow(std::function<void()> content); // Spawns a fullscreen window wrapping content
	static void CentreText(const std::string& text); // Draws some text, vertically centred
	static void CentrePosition(ImVec2 component_size); // Sets x position to ensure that component is horizontally centred according to size
	static bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags = 0); // Input text box that works with std::string and supports dynamic resizing
	static bool YSpacing(float pixels); // Adds vertical spacing of specified pixel amount
	static bool XSpacing(float pixels); // Adds horizontal spacing of specified pixel amount
	static bool WrapComponent(const std::string& id, ImVec2 component_size, std::function<void()> content); // Enforces component_size and ID for the next component to be drawn
};