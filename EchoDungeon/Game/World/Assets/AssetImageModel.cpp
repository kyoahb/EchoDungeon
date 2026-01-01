#include "AssetImageModel.h"

AssetImageModel::AssetImageModel(
	const AssetImage& asset_image
) {
	Mesh mesh = GenMeshPlane(1.0f, 1.0f, 1, 1);  // 1x1 quad mesh
	model = LoadModelFromMesh(mesh);
	model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = asset_image.get();  // Apply texture
}

const raylib::Model& AssetImageModel::get() const {
	return model;
}