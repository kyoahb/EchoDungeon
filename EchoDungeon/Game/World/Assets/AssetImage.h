#pragma once
#include "Imports/common.h"


class AssetImage {
public:
	AssetImage(
		const std::string& image_path
	);
	raylib::Texture2D texture;

	const raylib::Texture2D& get() const;
};
