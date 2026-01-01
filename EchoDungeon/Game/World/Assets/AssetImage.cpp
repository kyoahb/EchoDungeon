#include "AssetImage.h"

AssetImage::AssetImage(
	const std::string& image_path
) : texture(raylib::Texture2D(image_path)) {
	// Load image in

	TRACE("AssetImage: Loaded image from " + image_path);
}

const raylib::Texture2D& AssetImage::get() const {
	return texture;
}