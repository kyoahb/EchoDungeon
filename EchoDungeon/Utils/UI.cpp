#include "UI.h"
#include <imgui.h>
#include <raylib.h>  // For GetScreenWidth, GetScreenHeight
#include <string>

struct InputTextCallback_UserData
{
    std::string* Str;
    ImGuiInputTextCallback  ChainCallback;
    void* ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
    InputTextCallback_UserData* user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        std::string* str = user_data->Str;
        IM_ASSERT(data->Buf == str->c_str());
        str->resize(data->BufTextLen);
        data->Buf = (char*)str->c_str();
    }
    else if (user_data->ChainCallback)
    {
        data->UserData = user_data->ChainCallbackUserData;
        return user_data->ChainCallback(data);
    }
    return 0;
}

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

bool UIUtils::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0); // Allows the function to manage resizing, necessary for std::string
    flags |= ImGuiInputTextFlags_CallbackResize;
    
	InputTextCallback_UserData cb_user_data; // Callback type containing std::string pointer (str)
    cb_user_data.Str = str;
    cb_user_data.ChainCallback = NULL;
    cb_user_data.ChainCallbackUserData = NULL;

	// The final call to ImGui::InputText, passing in the string's c_str() buffer and capacity+1 (considering true size of the buffer)
    return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool UIUtils::YSpacing(float pixels) {
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + pixels); // Move cursor down by specified pixels
    return true;
}

bool UIUtils::XSpacing(float pixels) {
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pixels); // Move cursor right by specified pixels
    return true;
}

bool UIUtils::WrapComponent(const std::string& id, ImVec2 component_size, std::function<void()> content) {
	// Push ID and item width for the content that is about to be drawn
    ImGui::PushID(id.c_str());
    ImGui::PushItemWidth(component_size.x);

    // Draw content
	content();

	// Pop item width and ID after drawing
	ImGui::PopItemWidth();
	ImGui::PopID();
    return true;
}