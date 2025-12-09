#pragma once

#include <chrono>

class NetUtils {
public:
	/**
	* Returns the current time in milliseconds since the epoch
	*
	* @return uint64_t Time since epoch (ms)
	*/
	static uint64_t get_current_time_millis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
	}

	/**
	* Returns the current time in milliseconds since some other time
	*
	* @return uint64_t Time (ms)
	*/
	static uint64_t get_time_since(uint64_t start_time) {
		return get_current_time_millis() - start_time;
	}
};

