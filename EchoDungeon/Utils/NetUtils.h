#pragma once
#include "Imports/common.h"
#include <chrono>
#include <curl/curl.h>
#include <algorithm>
#include <cctype>

class NetUtils {
private:
	static size_t curl_write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
		((std::string*)userp)->append((char*)contents, size * nmemb);
		return size * nmemb;
	}

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

	/**
	* @brief Gets the external IP Address string of the local user, by requesting it from an external server
	* @return std::string External IP Address
	*/
	static std::string get_external_ip_string() {
		// Ensure curl is globally initialized (safe to call multiple times)
		static bool initialized = []() {
			curl_global_init(CURL_GLOBAL_DEFAULT);
			return true;
		}();
		(void)initialized; // Suppress unused warning

		CURL* curl = curl_easy_init();
		if (!curl) {
			return "unknown";
		}

		std::string response;
		curl_easy_setopt(curl, CURLOPT_URL, "http://api.ipify.org");
		curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4); // Force IPv4
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // 10 second timeout

		CURLcode res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			return "unknown";
		}

		return response;
	}

};