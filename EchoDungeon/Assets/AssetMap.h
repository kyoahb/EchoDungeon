#pragma once
#include "Imports/common.h"
#include <unordered_map>

static class AssetMap {
public:
	static const std::unordered_map<std::string, std::string> asset_paths;
	// Paths of files in Assets/files/ directory
	// Handed out relative to base game directory

	static std::string get_path(const std::string& asset_name);
};