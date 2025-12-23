#pragma once

#include "Imports/common.h"
#include <vector>
#include <string>
#include <unordered_map>

class Input {
public:
	static void update();

	static const std::vector<int>& get_keys_down(); // Returns the keys currently held down
	static const std::unordered_map<int, float>& get_keys_down_times(); // Returns the keys currently held down and times
	static const std::vector<int>& get_keys_pressed(); // Returns the keys pressed this frame
	static const std::vector<int>& get_keys_released(); // Returns the keys released this frame

	
	static bool is_key_down(int key); // Returns true if the key is currently held down
	static bool is_key_pressed(int key); // Returns true if the key was pressed this frame
	static bool is_key_released(int key); // Returns true if the key was released this frame

	static bool is_key_repeating(int key); // Returns true if the key is repeating (held for >KEY_REPEAT_DELAY)
	static std::vector<int>& get_keys_repeating(); // Returns the keys repeating (held for >KEY_REPEAT_DELAY)

	static bool is_shift_down(); // Returns true if shift is held down
	static bool is_caps_lock_on(); // Returns true if caps lock is on

	static std::string get_key_name(int key); // Returns the name of the key
	static std::string get_character(int key); // Returns the character of the key (takes shift into account)

	static bool is_printable(int key); // Returns true if the key is a printable character

private:
	static const float KEY_REPEAT_DELAY; // The delay before a key starts repeating (seconds)
	static const float KEY_REPEAT_RATE; // The rate at which keys repeat (characters per second)


	static bool caps_lock_down; // Stores the state of the caps lock key

	static std::unordered_map<int, std::string> caps_map; // Maps key codes to names when caps lock is on
	static std::unordered_map <int, std::string> shift_map; // Maps key codes to names when shift is held down
	static std::unordered_map<int, std::string> key_map; // Maps key codes to names
	static std::unordered_map<int, std::string> true_key_map; // Maps key codes to names (no special cases, prefixed)

	static std::vector<int> keys_pressed; // Stores the keys pressed (this frame)
	static std::unordered_map<int, float> keys_down_times; // Stores the down pressed this frame and times
	static std::vector<int> keys_released; // Stores the keys released this frame
};
