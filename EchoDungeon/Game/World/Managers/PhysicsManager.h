#pragma once
#include "Imports/common.h"
#include "Game/World/Entities/ObjectTransform.h"
#include "Game/World/Entities/Object.h"
#include "Game/World/Entities/Player.h"
#include "Game/World/Entities/Enemy.h"

// Forward declarations
class ServerWorldManager;
class ClientWorldManager;

class PhysicsManager {
public:
	static void update(
		std::unordered_map<uint32_t, Player>* players,
		std::unordered_map<uint32_t, Enemy>* enemies,
		std::unordered_map<uint32_t, Object>* objects,
		ServerWorldManager* server_world_manager,
		ClientWorldManager* client_world_manager);

};