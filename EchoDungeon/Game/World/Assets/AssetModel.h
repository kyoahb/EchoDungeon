#pragma once
#include "Imports/common.h"


class AssetModel {
public:
	AssetModel(
		const std::string& model_path
	);
	raylib::Model model;

	const raylib::Model& get() const;

	void add_diffuse(const std::string& texture_path);
	void add_normal(const std::string& texture_path);
};
