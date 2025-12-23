#include "AssetMap.h"

const std::unordered_map<std::string, std::string> AssetMap::asset_paths = {
	{"black_circle", "Assets/files/images/black_circle.png"},
	// Add more assets as needed
};

std::string AssetMap::get_path(const std::string& asset_name) {
	 auto it = asset_paths.find(asset_name);
	 if (it != asset_paths.end()) {
		 return it->second;
	 }
	 else {
		 ERROR("Asset not found in AssetMap: " + asset_name);
		 return "";
	 }
}