#include "Input.h"

std::unordered_map<int, std::string> Input::true_key_map = {
    // Letters
    {KEY_A, "KEY_A"}, {KEY_B, "KEY_B"}, {KEY_C, "KEY_C"}, {KEY_D, "KEY_D"}, {KEY_E, "KEY_E"},
    {KEY_F, "KEY_F"}, {KEY_G, "KEY_G"}, {KEY_H, "KEY_H"}, {KEY_I, "KEY_I"}, {KEY_J, "KEY_J"},
    {KEY_K, "KEY_K"}, {KEY_L, "KEY_L"}, {KEY_M, "KEY_M"}, {KEY_N, "KEY_N"}, {KEY_O, "KEY_O"},
    {KEY_P, "KEY_P"}, {KEY_Q, "KEY_Q"}, {KEY_R, "KEY_R"}, {KEY_S, "KEY_S"}, {KEY_T, "KEY_T"},
    {KEY_U, "KEY_U"}, {KEY_V, "KEY_V"}, {KEY_W, "KEY_W"}, {KEY_X, "KEY_X"}, {KEY_Y, "KEY_Y"},
    {KEY_Z, "KEY_Z"},

    // Numbers
    {KEY_ZERO, "KEY_ZERO"}, {KEY_ONE, "KEY_ONE"}, {KEY_TWO, "KEY_TWO"}, {KEY_THREE, "KEY_THREE"},
    {KEY_FOUR, "KEY_FOUR"}, {KEY_FIVE, "KEY_FIVE"}, {KEY_SIX, "KEY_SIX"}, {KEY_SEVEN, "KEY_SEVEN"},
    {KEY_EIGHT, "KEY_EIGHT"}, {KEY_NINE, "KEY_NINE"},

    // Function keys
    {KEY_F1, "KEY_F1"}, {KEY_F2, "KEY_F2"}, {KEY_F3, "KEY_F3"}, {KEY_F4, "KEY_F4"},
    {KEY_F5, "KEY_F5"}, {KEY_F6, "KEY_F6"}, {KEY_F7, "KEY_F7"}, {KEY_F8, "KEY_F8"},
    {KEY_F9, "KEY_F9"}, {KEY_F10, "KEY_F10"}, {KEY_F11, "KEY_F11"}, {KEY_F12, "KEY_F12"},

    // Special keys
    {KEY_SPACE, "KEY_SPACE"},
    {KEY_ESCAPE, "KEY_ESCAPE"},
    {KEY_ENTER, "KEY_ENTER"},
    {KEY_TAB, "KEY_TAB"},
    {KEY_BACKSPACE, "KEY_BACKSPACE"},
    {KEY_INSERT, "KEY_INSERT"},
    {KEY_DELETE, "KEY_DELETE"},
    {KEY_RIGHT, "KEY_RIGHT"},
    {KEY_LEFT, "KEY_LEFT"},
    {KEY_DOWN, "KEY_DOWN"},
    {KEY_UP, "KEY_UP"},
    {KEY_PAGE_UP, "KEY_PAGE_UP"},
    {KEY_PAGE_DOWN, "KEY_PAGE_DOWN"},
    {KEY_HOME, "KEY_HOME"},
    {KEY_END, "KEY_END"},
    {KEY_CAPS_LOCK, "KEY_CAPS_LOCK"},
    {KEY_SCROLL_LOCK, "KEY_SCROLL_LOCK"},
    {KEY_NUM_LOCK, "KEY_NUM_LOCK"},
    {KEY_PRINT_SCREEN, "KEY_PRINT_SCREEN"},
    {KEY_PAUSE, "KEY_PAUSE"},

    // Keypad
    {KEY_KP_0, "KEY_KP_0"}, {KEY_KP_1, "KEY_KP_1"}, {KEY_KP_2, "KEY_KP_2"}, {KEY_KP_3, "KEY_KP_3"},
    {KEY_KP_4, "KEY_KP_4"}, {KEY_KP_5, "KEY_KP_5"}, {KEY_KP_6, "KEY_KP_6"}, {KEY_KP_7, "KEY_KP_7"},
    {KEY_KP_8, "KEY_KP_8"}, {KEY_KP_9, "KEY_KP_9"},
    {KEY_KP_DECIMAL, "KEY_KP_DECIMAL"}, {KEY_KP_DIVIDE, "KEY_KP_DIVIDE"},
    {KEY_KP_MULTIPLY, "KEY_KP_MULTIPLY"}, {KEY_KP_SUBTRACT, "KEY_KP_SUBTRACT"},
    {KEY_KP_ADD, "KEY_KP_ADD"}, {KEY_KP_ENTER, "KEY_KP_ENTER"},
    {KEY_KP_EQUAL, "KEY_KP_EQUAL"},

    // Modifiers
    {KEY_LEFT_SHIFT, "KEY_LEFT_SHIFT"}, {KEY_LEFT_CONTROL, "KEY_LEFT_CONTROL"},
    {KEY_LEFT_ALT, "KEY_LEFT_ALT"}, {KEY_LEFT_SUPER, "KEY_LEFT_SUPER"},
    {KEY_RIGHT_SHIFT, "KEY_RIGHT_SHIFT"}, {KEY_RIGHT_CONTROL, "KEY_RIGHT_CONTROL"},
    {KEY_RIGHT_ALT, "KEY_RIGHT_ALT"}, {KEY_RIGHT_SUPER, "KEY_RIGHT_SUPER"},

    // Punctuation
    {KEY_APOSTROPHE, "KEY_APOSTROPHE"}, {KEY_COMMA, "KEY_COMMA"}, {KEY_MINUS, "KEY_MINUS"},
    {KEY_PERIOD, "KEY_PERIOD"}, {KEY_SLASH, "KEY_SLASH"}, {KEY_SEMICOLON, "KEY_SEMICOLON"},
    {KEY_EQUAL, "KEY_EQUAL"}, {KEY_LEFT_BRACKET, "KEY_LEFT_BRACKET"}, {KEY_BACKSLASH, "KEY_BACKSLASH"},
    {KEY_RIGHT_BRACKET, "KEY_RIGHT_BRACKET"}, {KEY_GRAVE, "KEY_GRAVE"}
};

std::unordered_map<int, std::string> Input::key_map = {
    // Letters
    {KEY_A, "a"}, {KEY_B, "b"}, {KEY_C, "c"}, {KEY_D, "d"}, {KEY_E, "e"},
    {KEY_F, "f"}, {KEY_G, "g"}, {KEY_H, "h"}, {KEY_I, "i"}, {KEY_J, "j"},
    {KEY_K, "k"}, {KEY_L, "l"}, {KEY_M, "m"}, {KEY_N, "n"}, {KEY_O, "o"},
    {KEY_P, "p"}, {KEY_Q, "q"}, {KEY_R, "r"}, {KEY_S, "s"}, {KEY_T, "t"},
    {KEY_U, "u"}, {KEY_V, "v"}, {KEY_W, "w"}, {KEY_X, "x"}, {KEY_Y, "y"},
    {KEY_Z, "z"},

    // Numbers
    {KEY_ZERO, "0"}, {KEY_ONE, "1"}, {KEY_TWO, "2"}, {KEY_THREE, "3"},
    {KEY_FOUR, "4"}, {KEY_FIVE, "5"}, {KEY_SIX, "6"}, {KEY_SEVEN, "7"},
    {KEY_EIGHT, "8"}, {KEY_NINE, "9"},

    // Function keys
    {KEY_F1, "F1"}, {KEY_F2, "F2"}, {KEY_F3, "F3"}, {KEY_F4, "F4"},
    {KEY_F5, "F5"}, {KEY_F6, "F6"}, {KEY_F7, "F7"}, {KEY_F8, "F8"},
    {KEY_F9, "F9"}, {KEY_F10, "F10"}, {KEY_F11, "F11"}, {KEY_F12, "F12"},

    // Special keys
    {KEY_SPACE, " "},
    {KEY_TAB, " "},

    // Keypad
    {KEY_KP_0, "0"}, {KEY_KP_1, "1"}, {KEY_KP_2, "2"}, {KEY_KP_3, "3"},
    {KEY_KP_4, "4"}, {KEY_KP_5, "5"}, {KEY_KP_6, "6"}, {KEY_KP_7, "7"},
    {KEY_KP_8, "8"}, {KEY_KP_9, "9"},
    {KEY_KP_DECIMAL, "."}, {KEY_KP_DIVIDE, "\xC3\xB7"},
    {KEY_KP_MULTIPLY, "\xC3\x97"}, {KEY_KP_SUBTRACT, "-"},
    {KEY_KP_ADD, "+"}, {KEY_KP_ENTER, "KP_ENTER"},
    {KEY_KP_EQUAL, "="},

    // Punctuation
    {KEY_APOSTROPHE, "'"}, {KEY_COMMA, ","}, {KEY_MINUS, "-"},
    {KEY_PERIOD, "."}, {KEY_SLASH, "/"}, {KEY_SEMICOLON, ";"},
    {KEY_EQUAL, "="}, {KEY_LEFT_BRACKET, "["}, {KEY_BACKSLASH, "\\"},
    {KEY_RIGHT_BRACKET, "]"}, {KEY_GRAVE, "`"}
};

std::unordered_map<int, std::string> Input::shift_map = {
    // Uppercase letters
    {KEY_A, "A"}, {KEY_B, "B"}, {KEY_C, "C"}, {KEY_D, "D"}, {KEY_E, "E"},
    {KEY_F, "F"}, {KEY_G, "G"}, {KEY_H, "H"}, {KEY_I, "I"}, {KEY_J, "J"},
    {KEY_K, "K"}, {KEY_L, "L"}, {KEY_M, "M"}, {KEY_N, "N"}, {KEY_O, "O"},
    {KEY_P, "P"}, {KEY_Q, "Q"}, {KEY_R, "R"}, {KEY_S, "S"}, {KEY_T, "T"},
    {KEY_U, "U"}, {KEY_V, "V"}, {KEY_W, "W"}, {KEY_X, "X"}, {KEY_Y, "Y"},
    {KEY_Z, "Z"},

    // Shifted numbers
    {KEY_ZERO, ")"}, {KEY_ONE, "!"}, {KEY_TWO, "@"}, {KEY_THREE, "#"},
    {KEY_FOUR, "$"}, {KEY_FIVE, "%"}, {KEY_SIX, "^"}, {KEY_SEVEN, "&"},
    {KEY_EIGHT, "*"}, {KEY_NINE, "("},

    // Punctuation with shift
    {KEY_APOSTROPHE, "\""}, {KEY_COMMA, "<"}, {KEY_MINUS, "_"},
    {KEY_PERIOD, ">"}, {KEY_SLASH, "?"}, {KEY_SEMICOLON, ":"},
    {KEY_EQUAL, "+"}, {KEY_LEFT_BRACKET, "{"}, {KEY_BACKSLASH, "|"},
    {KEY_RIGHT_BRACKET, "}"}, {KEY_GRAVE, "~"},
};

std::unordered_map<int, std::string> Input::caps_map = {
    // Uppercase letters
    {KEY_A, "A"}, {KEY_B, "B"}, {KEY_C, "C"}, {KEY_D, "D"}, {KEY_E, "E"},
    {KEY_F, "F"}, {KEY_G, "G"}, {KEY_H, "H"}, {KEY_I, "I"}, {KEY_J, "J"},
    {KEY_K, "K"}, {KEY_L, "L"}, {KEY_M, "M"}, {KEY_N, "N"}, {KEY_O, "O"},
    {KEY_P, "P"}, {KEY_Q, "Q"}, {KEY_R, "R"}, {KEY_S, "S"}, {KEY_T, "T"},
    {KEY_U, "U"}, {KEY_V, "V"}, {KEY_W, "W"}, {KEY_X, "X"}, {KEY_Y, "Y"},
    {KEY_Z, "Z"},
};

std::vector<int> Input::keys_pressed;
std::unordered_map<int, float> Input::keys_down_times;
std::vector<int> Input::keys_released;
bool Input::caps_lock_down = false;

const float Input::KEY_REPEAT_DELAY = 0.5f; // Seconds
const float Input::KEY_REPEAT_RATE = 20.0f; // Characters per second

void Input::update() {

    float deltaTime = GetFrameTime();

    // Clear previous frame's key states
    keys_pressed.clear();
    keys_released.clear();

    for (int i = 0; i < 512; i++) {
        if (keys_down_times.find(i) != keys_down_times.end()) {
            keys_down_times[i] += deltaTime; // Increment hold time
        }
        else if (IsKeyPressed(i)) { // If being held, cannot be pressed without being released
            keys_pressed.push_back(i);
            keys_down_times[i] = 0.0f;
        }

        if (IsKeyReleased(i)) {
            keys_released.push_back(i);
            keys_down_times.erase(i); // No longer held

        }
    }

    // Update caps lock state
    if (IsKeyPressed(KEY_CAPS_LOCK)) {
        caps_lock_down = !caps_lock_down;
    }

}

bool Input::is_key_down(int key) {
    return keys_down_times.find(key) != keys_down_times.end();
}

bool Input::is_key_pressed(int key) {
    return std::find(keys_pressed.begin(), keys_pressed.end(), key) != keys_pressed.end();
}

bool Input::is_key_released(int key) {
    return std::find(keys_released.begin(), keys_released.end(), key) != keys_released.end();
}


const std::vector<int>& Input::get_keys_down() {
    static std::vector<int> keys_down;
    keys_down.clear();
    for (const auto& pair : keys_down_times) {
        keys_down.push_back(pair.first);
    }

    // Return the keys that are currently down
    return keys_down;
}

const std::unordered_map<int, float>& Input::get_keys_down_times() {
    return keys_down_times;
}

const std::vector<int>& Input::get_keys_pressed() {
    return keys_pressed;
}

const std::vector<int>& Input::get_keys_released() {
    return keys_released;
}

std::string Input::get_key_name(int key) {
    auto it = true_key_map.find(key);
    if (it != true_key_map.end()) {
        return it->second;
    }
    return "UNKNOWN";
}

std::string Input::get_character(int key) {
    // This function will try find the most human-readable character for the key
    // Order:
    // Shift-modified printable keys (A), (!)
    // Caps-modified printable keys (A)
    // Printable keys (a)
    // True key names (KEY_A)

    if (is_shift_down()) { // Shift-modified printable keys
        auto it = shift_map.find(key);
        if (it != shift_map.end()) {
            return it->second;
        }
    }
    else if (is_caps_lock_on()) { // Caps-modified printable keys
        auto it = caps_map.find(key);
        if (it != caps_map.end()) {
            return it->second;
        }
    }
    else if (is_printable(key)) { // Printable keys
        auto it = key_map.find(key);
        if (it != key_map.end()) {
            return it->second;
        }
    }
    else { // True key names
        return get_key_name(key);
    }
}

bool Input::is_shift_down() {
    return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
}

bool Input::is_caps_lock_on() {
    return caps_lock_down;
}

bool Input::is_printable(int key) {
    return key >= 32 && key <= 126;
}

bool Input::is_key_repeating(int key) {
    auto it = keys_down_times.find(key);
    if (it == keys_down_times.end()) {
        return false; // Not held
    }

    float hold_time = it->second;

    if (hold_time > KEY_REPEAT_DELAY) {
        float time_repeating = hold_time - KEY_REPEAT_DELAY; // Time the key has been held within repeat range
        float characters_repeated = time_repeating * KEY_REPEAT_RATE; // Float number of characters repeated so far

        // Get the integer part (number of full repeats so far)
        int previous_repeats = static_cast<int>(characters_repeated);

        // Calculate what the count would be after this frame's time is added
        float delta_time = GetFrameTime();
        float next_characters_repeating = (time_repeating + delta_time) * KEY_REPEAT_RATE;
        int next_repeats = static_cast<int>(next_characters_repeating);

        // If adding this frame's time crosses a repeat boundary, trigger a repeat
        return next_repeats > previous_repeats;
    }
    return false; // Not repeating
}

std::vector<int>& Input::get_keys_repeating() {
    static std::vector<int> keys_repeating;
    keys_repeating.clear();

    for (const auto& pair : keys_down_times) {
        if (is_key_repeating(pair.first)) {
            keys_repeating.push_back(pair.first);
        }
    }
    return keys_repeating;
}