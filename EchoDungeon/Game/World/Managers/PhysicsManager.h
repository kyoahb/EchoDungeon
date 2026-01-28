#pragma once
#include "Imports/common.h"
#include "Game/World/Entities/ObjectTransform.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"


class PhysicsManager {
public:
	static void update(
		std::unordered_map<uint16_t, Player>* players, 
		std::unordered_map<uint16_t, Object>* objects);

};