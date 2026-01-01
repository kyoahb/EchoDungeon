# World Synchronization Guide

## Overview

This document explains the world synchronization system for EchoDungeon, including packet types, usage patterns, and implementation examples.

## Packet Types

### 1. WorldSnapshotPacket (ID: 10)
**Purpose**: Full world state synchronization  
**Direction**: Server ? Client  
**Reliability**: Reliable  
**Frequency**: Once on join, or on-demand for resync

**Contains**:
- All players in the world (including transforms, health, names)
- All objects in the world (including transforms, asset references)

**When to send**:
- Client joins the game world
- Client requests a full resync (after disconnect/lag)
- New game round starts

**Server example**:
```cpp
// When a client joins the world
WorldSnapshotPacket snapshot(world_manager.get_all_players(), world_manager.get_all_objects());
enet_peer_send(peer, 0, snapshot.to_enet_packet());
```

**Client example** (in World.cpp event handler):
```cpp
ClientEvents::WorldSnapshotEvent::subscribe([this](const ClientEvents::WorldSnapshotEventData& data) {
    // Clear existing world state
    players.clear();
    objects.clear();
    
    // Load all players
    for (const auto& [id, player] : data.packet.players) {
        players[id] = player;
    }
    
    // Load all objects and reconstruct asset pointers
    for (const auto& [id, object] : data.packet.objects) {
        Object obj = object;
        obj.ReconstructAsset();  // Important: Reconnect asset pointers
        objects[id] = obj;
    }
});
```

---

### 2. EntityUpdatePacket (ID: 11)
**Purpose**: Lightweight position/rotation updates  
**Direction**: Server ? Client  
**Reliability**: Unreliable (for performance)  
**Frequency**: Every server tick (20-60 Hz)

**Contains**:
- List of entity updates (only entities that changed)
- Transform data (position, rotation, scale)
- Health values for players
- Sequence number (optional, for ordering)

**When to send**:
- Every server tick (use a timer)
- Only include entities that moved/changed
- Skip sleeping/static entities

**Server example** (in server update loop):
```cpp
// Send every 50ms (20 updates/sec)
static auto last_update = std::chrono::steady_clock::now();
auto now = std::chrono::steady_clock::now();
auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update);

if (elapsed.count() >= 50) {
    std::vector<EntityUpdateData> updates;
    
    // Add player updates
    for (auto& [id, player] : players) {
        EntityUpdateData update;
        update.entity_id = std::to_string(id);
        update.transform = player.transform;
        update.health = player.health;
        updates.push_back(update);
    }
    
    // Add object updates (only moving objects)
    for (auto& [id, object] : objects) {
        if (!object.transform.get_is_static()) {
            EntityUpdateData update;
            update.entity_id = id;
            update.transform = object.transform;
            updates.push_back(update);
        }
    }
    
    // Send to all clients
    EntityUpdatePacket packet(updates, ++sequence_counter);
    for (auto& peer : connected_peers) {
        enet_peer_send(peer, 0, packet.to_enet_packet());
    }
    
    last_update = now;
}
```

**Client example**:
```cpp
ClientEvents::EntityUpdateEvent::subscribe([this](const ClientEvents::EntityUpdateEventData& data) {
    for (const auto& update : data.packet.updates) {
        // Check if it's a player
        try {
            uint16_t player_id = std::stoi(update.entity_id);
            if (players.count(player_id)) {
                players[player_id].transform = update.transform;
                players[player_id].health = update.health;
                continue;
            }
        } catch (...) {}
        
        // Otherwise it's an object
        if (objects.count(update.entity_id)) {
            objects[update.entity_id].transform = update.transform;
        }
    }
});
```

---

### 3. EntitySpawnPacket (ID: 12)
**Purpose**: Spawn a new object in the world  
**Direction**: Server ? Client  
**Reliability**: Reliable  
**Frequency**: On-demand

**Contains**:
- Full Object data (id, asset_id, type, transform)

**When to send**:
- New object created (door opened, item dropped, projectile fired)
- Player creates/places an object

**Server example**:
```cpp
// When spawning a chest
Object chest = Object::FromModel("chest");
chest.id = generate_unique_id();
chest.transform.set_position({10.0f, 0.0f, 5.0f});

// Add to server world
world_manager.add_object(chest);

// Broadcast to all clients
EntitySpawnPacket packet(chest);
server.broadcast(packet.to_enet_packet());
```

**Client example**:
```cpp
ClientEvents::EntitySpawnEvent::subscribe([this](const ClientEvents::EntitySpawnEventData& data) {
    Object obj = data.packet.object;
    obj.ReconstructAsset();  // Reconnect asset pointer
    objects[obj.id] = obj;
});
```

---

### 4. EntityDestroyPacket (ID: 13)
**Purpose**: Remove an entity from the world  
**Direction**: Server ? Client  
**Reliability**: Reliable  
**Frequency**: On-demand

**Contains**:
- Entity ID to destroy

**When to send**:
- Object destroyed (chest opened, item picked up, projectile hit)
- Cleanup old entities

**Server example**:
```cpp
// When destroying an object
std::string object_id = "chest_123";
world_manager.remove_object(object_id);

// Notify all clients
EntityDestroyPacket packet(object_id);
server.broadcast(packet.to_enet_packet());
```

**Client example**:
```cpp
ClientEvents::EntityDestroyEvent::subscribe([this](const ClientEvents::EntityDestroyEventData& data) {
    objects.erase(data.packet.entity_id);
});
```

---

### 5. PlayerSpawnPacket (ID: 14)
**Purpose**: Spawn a new player in the world  
**Direction**: Server ? Client  
**Reliability**: Reliable  
**Frequency**: On-demand

**Contains**:
- Full Player data (id, name, transform, health)

**When to send**:
- New player joins the game world
- Player respawns after death

**Server example**:
```cpp
// When a player joins
Player new_player(peer_id, false, username);
new_player.transform.set_position({0.0f, 1.0f, 0.0f});

// Add to server
world_manager.add_player(new_player);

// Broadcast to all clients
PlayerSpawnPacket packet(new_player);
server.broadcast(packet.to_enet_packet());
```

**Client example**:
```cpp
ClientEvents::PlayerSpawnEvent::subscribe([this](const ClientEvents::PlayerSpawnEventData& data) {
    Player player = data.packet.player;
    // Note: Player asset_model reference needs special handling due to const reference
    players[player.id] = player;
});
```

---

## Optimization Tips

### 1. Delta Compression
Only send what changed:
```cpp
// Track previous state
std::unordered_map<std::string, ObjectTransform> previous_transforms;

// In update loop, only send if changed
for (auto& [id, object] : objects) {
    if (previous_transforms[id] != object.transform) {
        // Add to update packet
        previous_transforms[id] = object.transform;
    }
}
```

### 2. Spatial Partitioning
Only send entities near the player:
```cpp
// Only send entities within 50 units
const float UPDATE_RADIUS = 50.0f;

for (auto& [peer, peer_player] : peer_to_player) {
    std::vector<EntityUpdateData> updates;
    
    for (auto& [id, object] : objects) {
        float distance = Vector3Distance(
            peer_player.transform.get_position(),
            object.transform.get_position()
        );
        
        if (distance <= UPDATE_RADIUS) {
            // Add to update for this peer
        }
    }
}
```

### 3. Update Rate Tiers
Different update rates for different entity types:
```cpp
// Players: 60 Hz (16ms)
// Projectiles: 30 Hz (33ms)
// Static objects: 5 Hz (200ms) or on-change only
```

### 4. Sequence Numbers
Handle out-of-order packets:
```cpp
std::unordered_map<std::string, uint32_t> last_sequence;

// In EntityUpdate handler
if (data.packet.sequence_number > last_sequence[entity_id]) {
    // Apply update
    last_sequence[entity_id] = data.packet.sequence_number;
} else {
    // Discard old packet
}
```

---

## Common Patterns

### Server Authority Pattern
Server is authoritative, clients are predictive:

**Client** (in Player.cpp):
```cpp
// Client predicts movement locally
if (is_local) {
    local_move(delta);  // Immediate feedback
    send_input_to_server();  // Tell server what we did
}
```

**Server** (validates and broadcasts):
```cpp
// Server receives input, validates, then broadcasts
void handle_player_input(PlayerInputPacket& packet, ENetPeer* peer) {
    Player& player = get_player(peer);
    player.move(validate_input(packet.movement));
    // Will be included in next EntityUpdate broadcast
}
```

### Client-side Prediction + Reconciliation
```cpp
// Client keeps history of inputs
std::vector<PlayerInput> input_history;

// When server update arrives
void reconcile(const EntityUpdateData& server_state) {
    if (is_different(local_state, server_state)) {
        // Snap to server position
        transform = server_state.transform;
        
        // Replay inputs since server update
        for (auto& input : input_history) {
            if (input.timestamp > server_state.timestamp) {
                apply_input(input);
            }
        }
    }
}
```

---

## Troubleshooting

### Objects not appearing
- Check `ReconstructAsset()` is called after deserialization
- Verify asset_id matches an entry in AssetMap
- Check object is within rendering distance

### Position desync
- Ensure server is authoritative
- Check sequence numbers
- Verify transform serialization includes all fields

### Performance issues
- Lower EntityUpdate frequency (increase tick interval)
- Implement spatial partitioning
- Use delta compression
- Profile packet sizes

---

## Next Steps

1. Implement event handlers in World.cpp
2. Add server-side world manager with entity tracking
3. Add player input packet (client -> server)
4. Implement client-side prediction
5. Add interpolation for smooth movement
