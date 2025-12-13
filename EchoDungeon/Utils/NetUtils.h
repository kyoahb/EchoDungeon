#pragma once
#include "Imports/common.h"
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

	/*
	* @brief Gets the IP Address string from an ENetAddress
	*/
	static std::string get_ip_string(const ENetAddress& address) {
		char ip_str[INET6_ADDRSTRLEN]; // Reserve space for an IP address
		enet_address_get_host_ip(&address, ip_str, sizeof(ip_str)); // Get the IP address string
		
		std::string ip = std::string(ip_str); // Convert to std::string
		if (ip.empty()) { // If empty, return "unknown"
			return "unknown";
		}
		return ip;
	}
};

