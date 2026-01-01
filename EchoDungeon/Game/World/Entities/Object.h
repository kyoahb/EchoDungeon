#pragma once
#include "Imports/common.h"
#include "ObjectTransform.h"
#include "Game/World/Assets/AssetMap.h"
#include <variant>

enum class ObjectType : uint8_t {
    MODEL = 1,           // 3D model
    IMAGE_MODEL = 2,     // Billboard/quad model
};

class Object {
public:
   Object(uint16_t id, const std::string& asset_id, ObjectType type);

   Object();
   virtual ~Object() = default;

   // Core properties
   ObjectTransform transform;
   uint16_t id;  // Unique object ID
   std::string asset_id;  // Asset reference ID

   // Get the type
   ObjectType GetType() const { return type; }

   void Draw(const raylib::Camera3D& camera) const;

   template <typename Archive>
   void archive(Archive& archive) const {
       uint8_t type_value = static_cast<uint8_t>(type);
       archive(id, asset_id, type_value, transform);
   }

   template <typename Archive>
   void load(Archive& archive) {
       uint8_t type_value;
       archive(id, asset_id, type_value, transform);
       type = static_cast<ObjectType>(type_value);
   }

private:
    ObjectType type;

};