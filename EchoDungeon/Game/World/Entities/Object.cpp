#include "Object.h"
#include "Utils/MathUtils.h"
Object::Object()
    : asset_id(""), type(ObjectType::MODEL), id(65534) {
}

Object::Object(uint32_t id, const std::string& asset_id, ObjectType type)
    : asset_id(asset_id), type(type), id(id) {
}

void Object::Draw(const raylib::Camera3D& camera) const {
    AxisAngle rot = MathUtils::EulerToAxisAngle(transform.get_rotation());
    if (type == ObjectType::MODEL) {  
        AssetMap::get_model(asset_id).get().Draw(
            transform.get_position(),
            rot.axis,
            rot.angle,
            transform.get_scale(),
            color
        );
    }
    else if (type == ObjectType::IMAGE_MODEL) {
        AssetMap::get_image_model(asset_id).get().Draw(
            transform.get_position(),
            rot.axis,
            rot.angle,
            transform.get_scale(),
            color
        );
    }
}
