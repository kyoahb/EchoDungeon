#pragma once

#include <functional>
#include <string>
#include <imgui.h>

class UIUtils {
public:
	static void FullscreenWindow(std::function<void()> content); // Spawns a fullscreen window wrapping content
	static void CentreText(const std::string& text); // Draws some text, vertically centred
	static void CentrePosition(ImVec2 component_size); // Sets x position to ensure that component is horizontally centred according to size
};