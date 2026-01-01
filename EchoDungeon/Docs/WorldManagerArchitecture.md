# World Manager Architecture

## Overview

The world state management is split into two separate managers:
- **`ServerWorldManager`**: Authoritative server-side world state
- **`ClientWorldManager`**: Client-side world state with rendering

This separation provides clean architecture even for the **hosting client** (which runs both a server and a client).

---

## ServerWorldManager

### Responsibilities
1. **Authoritative State** - The single source of truth for all entities
2. **Input Validation** - Validates all player input (anti-cheat, collision)
3. **Broadcasting Updates** - Sends world state to all clients
4. **Physics Simulation** - Applies physics to all entities (when PhysicsManager is ready)

### Key Methods

```cpp
// Player management
void AddPlayer(uint16_t peer_id, const std::string& name);
void RemovePlayer(uint16_t peer_id);
void HandlePlayerInput(uint16_t peer_id, const ObjectTransform& input_transform);

// Object management
std::string SpawnObject(ObjectType type, const std::string& asset_id, const raylib::Vector3& position);
void DestroyObject(const std::string& object_id);

// Network synchronization
void BroadcastWorldSnapshot();  // Full state (on join)
void BroadcastEntityUpdates();  // Delta updates (20 Hz)
```

### Usage Example (Server-side)

```cpp
// In Server::on_player_connected(ENetPeer* peer)
uint16_t peer_id = get_peer_id(peer);
server_world_manager->AddPlayer(peer_id, "PlayerName");
server_world_manager->BroadcastWorldSnapshot(peer);  // Send full state to new player

// In Server::update()
server_world_manager->Update(delta_time);  // Automatically broadcasts updates at 20 Hz

// When receiving PlayerInputPacket
ServerEvents::PlayerInputEvent::subscribe([this](const ServerEvents::PlayerInputEventData& data) {
    uint16_t peer_id = get_peer_id(data.peer);
    server_world_manager->HandlePlayerInput(peer_id, data.packet.transform);
});

// Spawn an object
std::string chest_id = server_world_manager->SpawnObject(
    ObjectType::MODEL, 
    "chest", 
    {10.0f, 0.0f, 5.0f}
);
```

---

## ClientWorldManager

### Responsibilities
1. **Rendering** - Draws all entities in 3D and 2D
2. **Local Input** - Processes local player input and sends to server
3. **State Updates** - Receives and applies updates from server
4. **Camera Management** - Follows local player
5. **Client-side Prediction** - Applies local player movement immediately for responsiveness

### Key Methods

```cpp
// Local player
void SetLocalPlayer(uint16_t peer_id);
Player* GetLocalPlayer();

// Network synchronization
void ApplyWorldSnapshot(const WorldSnapshotPacket& snapshot);
void ApplyEntityUpdates(const std::vector<EntityUpdateData>& updates);
void SendLocalPlayerInput();  // Sends to server

// Rendering
void Draw3D(const raylib::Camera3D& camera);
void Draw2D(const raylib::Camera3D& camera);
```

### Usage Example (Client-side)

```cpp
// In Client initialization
client_world_manager = std::make_unique<ClientWorldManager>(client.get());

// When receiving connection confirmation
ClientEvents::ConnectionConfirmationEvent::subscribe([this](const auto& data) {
    client_world_manager->SetLocalPlayer(data.packet.your_peer_id);
});

// When receiving world snapshot (on join)
ClientEvents::WorldSnapshotEvent::subscribe([this](const auto& data) {
    client_world_manager->ApplyWorldSnapshot(data.packet);
});

// When receiving entity updates (every server tick)
ClientEvents::EntityUpdateEvent::subscribe([this](const auto& data) {
    client_world_manager->ApplyEntityUpdates(data.packet.updates);
});

// In game loop
void World::update() {
    client_world_manager->Update(GetFrameTime());  // Handles input & sending
    
    BeginMode3D(client_world_manager->GetCamera());
    client_world_manager->Draw3D(client_world_manager->GetCamera());
    EndMode3D();
    
    client_world_manager->Draw2D(client_world_manager->GetCamera());
}
```

---

## Hosting Client Architecture

When a client hosts a server, **both managers run simultaneously**:

```
Hosting Client Process:
??? Server (Port 7777)
?   ??? ServerWorldManager (authoritative state)
?       ??? Validates all input
?       ??? Broadcasts updates to all clients
?       ??? Simulates physics
?
??? Client (connects to localhost:7777)
    ??? ClientWorldManager (rendering & local input)
        ??? Processes local player input
        ??? Sends input to server
        ??? Receives updates from server
        ??? Renders world state

Flow:
1. Local player presses W
2. ClientWorldManager applies movement immediately (prediction)
3. ClientWorldManager sends PlayerInputPacket to server
4. ServerWorldManager validates & applies movement
5. ServerWorldManager broadcasts EntityUpdatePacket to all clients
6. ClientWorldManager receives update (may correct prediction if needed)
```

### Why This Separation?

1. **Server Authority** - Even the host's input goes through validation
2. **Consistency** - Host sees the same thing as other clients
3. **Testing** - Can test server logic without rendering
4. **Security** - Host can't cheat any easier than remote clients
5. **Future-proofing** - Easy to split into dedicated server later

---

## Update Flow

### Server (20 Hz - every 50ms)
```cpp
ServerWorldManager::Update(delta_time):
1. Check if update_interval elapsed (50ms)
2. Collect entity updates (players + dynamic objects)
3. Broadcast EntityUpdatePacket (unreliable)
4. Apply physics transformations
```

### Client (60 Hz - every frame)
```cpp
ClientWorldManager::Update(delta_time):
1. Process local player input (WASD)
2. Apply movement immediately (client-side prediction)
3. Check if input_send_interval elapsed (~16ms)
4. If local player moved, send PlayerInputPacket
5. Update camera to follow player
6. Apply physics transformations
```

---

## Client-Side Prediction

The client applies local player movement **immediately** without waiting for server confirmation:

```cpp
// Client-side
Player* local_player = GetLocalPlayer();
local_player->move(input);  // Apply immediately
SendLocalPlayerInput();     // Tell server

// When server update arrives
if (peer_id != local_player_id) {
    // Update other players
    player.transform = server_transform;
} else {
    // Don't overwrite local player (already predicted)
    // TODO: Add reconciliation if prediction was wrong
}
```

This provides responsive controls even with network latency.

---

## Physics Integration (Future)

Both managers will integrate with `PhysicsManager`:

```cpp
// Server
void ServerWorldManager::Update(float delta_time) {
    physics_manager.Update(delta_time);  // Simulate physics
    BroadcastEntityUpdates();            // Send results to clients
}

// Client
void ClientWorldManager::Update(float delta_time) {
    physics_manager.Update(delta_time);  // Simulate physics locally
    // Server will correct if client prediction was wrong
}
```

---

## Event Subscriptions

### Server Events

```cpp
// In Server initialization
ServerEvents::PlayerInputEvent::subscribe([this](const auto& data) {
    uint16_t peer_id = get_peer_id(data.peer);
    server_world_manager->HandlePlayerInput(peer_id, data.packet.transform);
});

ServerEvents::DisconnectEvent::subscribe([this](const auto& data) {
    uint16_t peer_id = get_peer_id(data.event.peer);
    server_world_manager->RemovePlayer(peer_id);
});
```

### Client Events

```cpp
// In Client initialization
ClientEvents::WorldSnapshotEvent::subscribe([this](const auto& data) {
    client_world_manager->ApplyWorldSnapshot(data.packet);
});

ClientEvents::EntityUpdateEvent::subscribe([this](const auto& data) {
    client_world_manager->ApplyEntityUpdates(data.packet.updates);
});

ClientEvents::PlayerSpawnEvent::subscribe([this](const auto& data) {
    Player player = data.packet.player;
    // TODO: player.ReconstructAsset();
    client_world_manager->AddPlayer(player);
});

ClientEvents::EntitySpawnEvent::subscribe([this](const auto& data) {
    client_world_manager->AddObject(data.packet.object);
});

ClientEvents::EntityDestroyEvent::subscribe([this](const auto& data) {
    try {
        uint16_t player_id = std::stoi(data.packet.entity_id);
        client_world_manager->RemovePlayer(player_id);
    } catch (...) {
        client_world_manager->RemoveObject(data.packet.entity_id);
    }
});
```

---

## Performance Considerations

### Network Bandwidth

**EntityUpdatePacket** (20 Hz):
- Only includes **changed entities**
- Skips static objects
- Unreliable delivery (occasional loss is acceptable)
- ~100-500 bytes per packet depending on entity count

**PlayerInputPacket** (60 Hz):
- Only sent when local player **moves**
- Unreliable delivery
- ~50-100 bytes per packet

**WorldSnapshotPacket** (on-demand):
- Only sent when player joins
- Reliable delivery
- Size depends on world complexity

### Optimizations

1. **Delta Compression** - Only send changed entities
2. **Spatial Partitioning** - Only send entities near player (future)
3. **Movement Threshold** - Don't send input if movement < 0.01 units
4. **Static Object Filtering** - Skip static objects in updates

---

## Next Steps

1. **Integrate PhysicsManager** when ready
2. **Add prediction reconciliation** for smoother multiplayer
3. **Implement spatial partitioning** for large worlds
4. **Add interpolation** between server updates for smooth movement
5. **Fix Player asset serialization** (add `ReconstructAsset()`)
