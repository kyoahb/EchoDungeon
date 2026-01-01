#include "AssetModel.h"

AssetModel::AssetModel(
	const std::string& model_path
) : model(raylib::Model(model_path)) {
	// Load model in

	TRACE("AssetModel: Loaded model from " + model_path);
}

const raylib::Model& AssetModel::get() const {
	return model;
}

void AssetModel::add_diffuse(const std::string& texture_path) {
	raylib::Texture2D texture(texture_path);
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

	TRACE("AssetModel: Added diffuse texture from " + texture_path);
}

void AssetModel::add_normal(const std::string& texture_path) {
	raylib::Texture2D texture(texture_path);
	model.materials[0].maps[MATERIAL_MAP_NORMAL].texture = texture;

	TRACE("AssetModel: Added normal texture from " + texture_path);
}