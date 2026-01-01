#pragma once
#include "Imports/common.h"
#include "AssetImage.h"

class AssetImageModel {
public:
	AssetImageModel(
		const AssetImage& asset_image
	);
	raylib::Model model;

	const raylib::Model& get() const;
};
