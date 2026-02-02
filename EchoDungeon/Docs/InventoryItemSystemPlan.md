# Inventory & Item System Implementation Plan

## Overview
This document outlines the complete implementation plan for adding a server-authoritative inventory and item system to EchoDungeon. Items will have stat-modifying effects that are applied in order when picked up and recalculated when discarded.

---

## System Flow

### Item Drop on Enemy Death
1. Player attacks and kills an enemy (handled in `ServerWorldManager::handle_player_attack`)
2. Server runs RNG check for item drop
3. If successful, server creates a new item and assigns it to the player
4. Server broadcasts item pickup packet to all clients
5. Client receives packet and adds item to local player's inventory
6. Item effects are applied to player stats immediately upon pickup
7. Player can view inventory by pressing 'E' (ImGui window)
8. Player can discard items via 'X' button, triggering stat recalculation

---

## New Files to Create

### 1. **Networking/Packet/Instances/Item/ItemPickup.h**
```cpp
// Packet ID: 23
// Direction: Server -> Client
// Purpose: Notify client that player received an item
```
- **Fields:**
  - `uint32_t player_id` - Which player received the item
  - `Item item` - Full item data (id, name, description, effects, asset_id)

### 2. **Networking/Packet/Instances/Item/ItemDiscard.h**
```cpp
// Packet ID: 24
// Direction: Client -> Server
// Purpose: Request to discard an item from inventory
```
- **Fields:**
  - `uint32_t item_id` - ID of item to discard

### 3. **Game/World/Entities/Inventory.cpp**
- Implementation of `add_item()`, `remove_item()`, `draw_ui()`
- ImGui rendering for inventory window

### 4. **Game/World/Systems/ItemGenerator.h**
- Static class for generating random items with random stats
- Contains item templates/pools
- RNG logic for drop chances and stat ranges
- Considering which asset id to use for the item
('sword' for items that focus more on attack / attack speed boosts
'armour' for items that focus more on health / speed boosts are the two options)

### 5. **Game/World/Systems/ItemGenerator.cpp**
- Implementation of item generation logic

---

## Files to Update

### 1. **Game/World/Entities/Item.h**
**Changes:**
- Add cereal serialization to `Item` class
- Add cereal serialization to `ItemEffects` struct
- Ensure all fields can be serialized

**New Methods:**
```cpp
template <typename Archive>
void serialize(Archive& archive) {
    archive(id, asset_id, item_name, item_description, effects);
}
```

### 2. **Game/World/Entities/Inventory.h**
**Changes:**
- Add cereal serialization support
- Add method to get item by ID
- Add method to check if item exists

**New/Updated Methods:**
```cpp
bool has_item(uint32_t item_id) const;
Item* get_item(uint32_t item_id);
const std::vector<uint32_t>& get_item_ids() const;

template <typename Archive>
void serialize(Archive& archive) {
    archive(item_ids);
}
```

### 3. **Game/World/Entities/Inventory.cpp**
**New Implementation:**
- `add_item()` - Add item ID to inventory
- `remove_item()` - Remove item ID from inventory
- `draw_ui()` - ImGui window showing all items with discard buttons
- `has_item()` - Check if item exists
- `get_item()` - Get item pointer by ID

**ImGui UI Structure:**
```
┌─────────────────────────────────────┐
│ Inventory (Press E to toggle)      │
├─────────────────────────────────────┤
│ [Icon] Item Name              [X]   │
│        +10 Health, +5 Damage        │
│        Description text...          │
├─────────────────────────────────────┤
│ [Icon] Item Name              [X]   │
│        -5 Speed, +20 Max Health     │
│        Description text...          │
└─────────────────────────────────────┘
```

### 4. **Game/World/Entities/Player.h**
**Changes:**
- Add `Inventory inventory` member variable
- Add stat calculation tracking fields
- Update `serialize()` method to include inventory

**New Fields:**
```cpp
Inventory inventory;

// Base stats (before item modifications)
float base_damage = 10.0f;
float base_max_health = 100.0f;
float base_range = 2.0f;
float base_speed = 2.0f;
uint64_t base_attack_cooldown = 500;
```

**Updated serialize():**
```cpp
template <typename Archive>
void serialize(Archive& archive) {
    archive(id, is_local, name, transform, health, asset_id,
            damage, max_health, range, speed, attack_cooldown,
            base_damage, base_max_health, base_range, base_speed, 
            base_attack_cooldown, inventory);
}
```

**New Methods:**
```cpp
void recalculate_stats();  // Recalculate all stats from base + items
void apply_item_effects(const ItemEffects& effects);
void remove_item_effects(uint32_t item_id);  // Triggers full recalc
```

### 5. **Game/World/Entities/Player.cpp**
**New Implementation:**
```cpp
void Player::recalculate_stats() {
    // Reset to base stats
    damage = base_damage;
    max_health = base_max_health;
    range = base_range;
    speed = base_speed;
    attack_cooldown = base_attack_cooldown;
    
    // Apply all items in order
    for (uint32_t item_id : inventory.item_ids) {
        Item* item = /* get from item registry */;
        if (item) {
            apply_item_effects(item->effects);
        }
    }
    
    // Clamp health to new max_health
    if (health > max_health) {
        health = max_health;
    }
}

void Player::apply_item_effects(const ItemEffects& effects) {
    // Apply flat modifiers first
    damage += effects.damage_boost;
    max_health += effects.max_health_boost;
    range += effects.range_boost;
    speed += effects.speed_boost;
    attack_cooldown -= effects.atk_cooldown_reduction;
    
    // Apply percentage modifiers
    damage *= (1.0f + effects.damage_percentage_boost);
    max_health *= (1.0f + effects.max_health_percentage_boost);
    range *= (1.0f + effects.range_percentage_boost);
    speed *= (1.0f + effects.speed_percentage_boost);
    attack_cooldown *= (1.0f - effects.atk_cooldown_percent_reduction);
    
    // Apply healing (immediate effect)
    health += effects.healing;
    health += max_health * effects.healing_percentage;
    if (health > max_health) {
        health = max_health;
    }
}

void Player::remove_item_effects(uint32_t item_id) {
    inventory.remove_item(item_id);
    recalculate_stats();
}
```

### 6. **Game/World/Managers/ServerWorldManager.h**
**Changes:**
- Add item registry/storage
- Add item generation methods
- Add next item ID counter

**New Fields:**
```cpp
std::unordered_map<uint32_t, Item> items;  // All items in the world
uint32_t next_item_id = 1;

// Configuration
float item_drop_chance = 0.3f;  // 30% chance
```

**New Methods:**
```cpp
uint32_t create_item_for_player(uint32_t player_id);
void handle_item_discard(uint32_t player_id, uint32_t item_id);
Item* get_item(uint32_t item_id);
```

### 7. **Game/World/Managers/ServerWorldManager.cpp**
**Changes:**
- Modify `handle_player_attack()` to check for item drops when enemy dies
- Add item creation logic
- Add item discard handling

**Modified `handle_player_attack()`:**
```cpp
// After detecting enemy death (around line 254):
if (enemy.health <= 0.0f) {
    enemies_to_destroy.push_back(enemy_id);
    
    // RNG for item drop
    float roll = (float)rand() / RAND_MAX;
    if (roll <= item_drop_chance) {
        create_item_for_player(peer_id);
    }
}
```

**New `create_item_for_player()`:**
```cpp
uint32_t ServerWorldManager::create_item_for_player(uint32_t player_id) {
    Player* player = get_player(player_id);
    if (!player) return 0;
    
    // Generate random item (use ItemGenerator)
    uint32_t item_id = next_item_id++;
    Item item = ItemGenerator::generate_random_item(item_id);
    
    // Store item in registry
    items[item_id] = item;
    
    // Add to player inventory
    player->inventory.add_item(item_id);
    
    // Apply item effects to player
    player->apply_item_effects(item.effects);
    
    // Broadcast item pickup to all clients
    ItemPickupPacket packet(player_id, item);
    server->broadcast_packet(packet);
    
    INFO("Player " + std::to_string(player_id) + 
         " received item: " + item.item_name);
    
    return item_id;
}
```

**New `handle_item_discard()`:**
```cpp
void ServerWorldManager::handle_item_discard(uint32_t player_id, uint32_t item_id) {
    Player* player = get_player(player_id);
    if (!player) return;
    
    // Check if player has this item
    if (!player->inventory.has_item(item_id)) {
        WARN("Player " + std::to_string(player_id) + 
             " tried to discard item they don't have: " + 
             std::to_string(item_id));
        return;
    }
    
    // Remove from inventory and recalculate stats
    player->remove_item_effects(item_id);
    
    // Remove from item registry
    items.erase(item_id);
    
    INFO("Player " + std::to_string(player_id) + 
         " discarded item: " + std::to_string(item_id));
    
    // Note: No packet broadcast needed, next PlayerUpdate will sync stats
}
```

### 8. **Game/World/Managers/ClientWorldManager.h**
**Changes:**
- Add item registry for client-side copies
- Add method to handle item pickup

**New Fields:**
```cpp
std::unordered_map<uint32_t, Item> items;  // Client-side item copies
bool show_inventory = false;  // Toggle for inventory UI
```

**New Methods:**
```cpp
void handle_item_pickup(uint32_t player_id, const Item& item);
void request_item_discard(uint32_t item_id);
void toggle_inventory();
Item* get_item(uint32_t item_id);
```

### 9. **Game/World/Managers/ClientWorldManager.cpp**
**Changes:**
- Add inventory UI rendering in `draw_2d()`
- Add input handling for 'E' key to toggle inventory
- Add item pickup handling

**Modified `update()`:**
```cpp
// Add inventory toggle check
if (IsKeyPressed(KEY_E)) {
    toggle_inventory();
}
```

**Modified `draw_2d()`:**
```cpp
// At end of method, add:
Player* local = get_local_player();
if (local && show_inventory) {
    local->inventory.draw_ui();
}
```

**New `handle_item_pickup()`:**
```cpp
void ClientWorldManager::handle_item_pickup(uint32_t player_id, const Item& item) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    
    // Store item in client registry
    items[item.id] = item;
    
    // Get player
    Player* player = get_player(player_id);
    if (!player) return;
    
    // Add to inventory
    player->inventory.add_item(item.id);
    
    // Apply effects
    player->apply_item_effects(item.effects);
    
    INFO("CLIENT: Player " + std::to_string(player_id) + 
         " picked up item: " + item.item_name);
}
```

**New `request_item_discard()`:**
```cpp
void ClientWorldManager::request_item_discard(uint32_t item_id) {
    ItemDiscardPacket packet(item_id);
    client->send_packet(packet);
    
    INFO("CLIENT: Requested discard of item: " + std::to_string(item_id));
}
```

### 10. **Game/Events/EventList.h**
**Changes:**
- Add event declarations for new packets

**Add to ServerEvents namespace:**
```cpp
// ItemDiscard Event (Packet ID: 24)
SERVER_PACKET_EVENT_DECLARATION(ItemDiscard)
```

**Add to ClientEvents namespace:**
```cpp
// ItemPickup Event (Packet ID: 23)
CLIENT_PACKET_EVENT_DECLARATION(ItemPickup)
```

### 11. **Networking/Packet/PacketRegistryInit.cpp**
**Changes:**
- Register new packet types

**Add to `initializeRegistry()`:**
```cpp
// Add to Client -> Server section (around line 66):
REGISTER_SERVER_PACKET(24, ItemDiscard);

// Add to Server -> Client section (around line 80):
REGISTER_CLIENT_PACKET(23, ItemPickup);
```

### 12. **Game/State/Instances/World.h**
**Changes:**
- Add subscription IDs for new events

**New Fields:**
```cpp
// Client event subscriptions
int client_item_pickup_sub = -1;

// Server event subscriptions
int server_item_discard_sub = -1;
```

### 13. **Game/State/Instances/World.cpp**
**Changes:**
- Add event subscription setup
- Add event handlers

**Add to `setup_client_events()`:**
```cpp
client_item_pickup_sub = ClientEvents::ItemPickupEvent::register_callback(
    [this](const ClientEvents::ItemPickupEventData& data) {
        c_world_manager->handle_item_pickup(
            data.packet.player_id,
            data.packet.item
        );
    }
);
```

**Add to `setup_server_events()`:**
```cpp
server_item_discard_sub = ServerEvents::ItemDiscardEvent::register_callback(
    [this](const ServerEvents::ItemDiscardEventData& data) {
        uint32_t player_id = game.server->peers.get_user_data(data.peer)->server_side_id;
        s_world_manager->handle_item_discard(
            player_id,
            data.packet.item_id
        );
    }
);
```

**Add to `on_deactivate()`:**
```cpp
// In client unsubscribe section:
if (client_item_pickup_sub != -1) {
    ClientEvents::ItemPickupEvent::unregister_callback(client_item_pickup_sub);
    client_item_pickup_sub = -1;
}

// In server unsubscribe section:
if (server_item_discard_sub != -1) {
    ServerEvents::ItemDiscardEvent::unregister_callback(server_item_discard_sub);
    server_item_discard_sub = -1;
}
```

### 14. **Networking/Packet/Instances/Player/PlayerSpawn.h**
**Changes:**
- Add inventory field to constructor and serialization

**Updated fields:**
```cpp
Inventory inventory;
```

**Updated constructor:**
```cpp
PlayerSpawnPacket(uint32_t _id, const std::string& _name, const ObjectTransform& _transform,
    float _health = 100.0f, float _max_health = 100.0f, float _damage = 10.0f,
    float _range = 2.0f, float _speed = 2.0f, uint64_t _attack_cooldown = 500,
    uint64_t _last_attack_time = 0, const std::string& _asset_id = "player",
    const Inventory& _inventory = Inventory())
    : Packet(14, true), id(_id), name(_name), transform(_transform),
      health(_health), max_health(_max_health), damage(_damage),
      range(_range), speed(_speed), attack_cooldown(_attack_cooldown),
      last_attack_time(_last_attack_time), asset_id(_asset_id), 
      inventory(_inventory) {
}
```

**Updated serialize:**
```cpp
template<class Archive>
void serialize(Archive& archive) {
    archive(header, is_reliable, id, name, transform, health, max_health, 
            damage, range, speed, attack_cooldown, last_attack_time, 
            asset_id, inventory);
}
```

### 15. **Networking/Packet/Instances/WorldSnapshot.h**
**Changes:**
- Ensure item data is included just in case of future changes

### 16. **Game/World/Managers/ServerWorldManager.cpp - add_player()**
**Changes:**
- Update PlayerSpawnPacket to include inventory

**Around line 51:**
```cpp
PlayerSpawnPacket packet(
    player.id,
    player.name,
    player.transform,
    player.health,
    player.max_health,
    player.damage,
    player.range,
    player.speed,
    player.attack_cooldown,
    player.last_attack_time,
    player.asset_id,
    player.inventory  // ADD THIS
);
```

### 17. **Game/World/Managers/ClientWorldManager.cpp - add_player()**
**Changes:**
- Ensure inventory is copied when adding players

**No code changes needed if using default copy constructor, but verify:**
```cpp
void ClientWorldManager::add_player(const Player& player) {
    std::lock_guard<std::mutex> lock(world_state_mutex);
    players[player.id] = player;  // This should copy inventory automatically
}
```

---

## Implementation Order

Follow this order to minimize compilation errors and allow incremental testing:

### Phase 1: Core Data Structures
1. Update `Item.h` with serialization
2. Update `Inventory.h` with new methods and serialization
3. Implement `Inventory.cpp`
4. Update `Player.h` with inventory and base stats
5. Implement stat calculation methods in `Player.cpp`

### Phase 2: Networking Layer
6. Create `ItemPickup.h` packet
7. Create `ItemDiscard.h` packet
8. Update `EventList.h` with new events
9. Update `PacketRegistryInit.cpp` with new packets

### Phase 3: Server Logic
10. Create `ItemGenerator.h` and `.cpp` (optional, can hardcode for testing)
11. Update `ServerWorldManager.h` with item registry
12. Implement item creation in `ServerWorldManager.cpp`
13. Modify `handle_player_attack()` for item drops
14. Implement `handle_item_discard()` in `ServerWorldManager.cpp`
15. Update `PlayerSpawn` packet to include inventory

### Phase 4: Client Logic
16. Update `ClientWorldManager.h` with item handling
17. Implement item pickup handling in `ClientWorldManager.cpp`
18. Implement inventory toggle and UI rendering
19. Add discard request functionality

### Phase 5: Integration
20. Update `World.cpp` event subscriptions
21. Test item drops on enemy kills
22. Test inventory UI display
23. Test item discard and stat recalculation
24. Test multiplayer synchronization

---

## Testing Checklist

- [ ] Item drops when enemy is killed (server-side)
- [ ] Item appears in player inventory immediately (client-side)
- [ ] Item effects are applied to player stats
- [ ] Multiple items stack effects correctly
- [ ] Inventory UI opens/closes with 'E' key
- [ ] Inventory UI shows all items with correct info
- [ ] Clicking 'X' button discards item
- [ ] Discarding item recalculates stats correctly
- [ ] Item order is preserved (affects calculation)
- [ ] Negative stat items work correctly
- [ ] Mixed positive/negative items work correctly
- [ ] Inventory persists through reconnection (via WorldSnapshot)
- [ ] Multiple players can each have different inventories
- [ ] Inventory UI only shows for local player

---

## Configuration Values

These can be tweaked for game balance:

```cpp
// ServerWorldManager
float item_drop_chance = 0.3f;  // 30% base drop rate

// ItemGenerator (if created)
struct ItemConfig {
    int min_damage_boost = -10;
    int max_damage_boost = 20;
    float min_speed_boost = -1.0f;
    float max_speed_boost = 2.0f;
    // etc...
};
```

---

## Notes and Considerations

1. **Stat Application Order**: Items are applied in the order they appear in `inventory.item_ids`. This order is maintained and never changes, ensuring consistent recalculation.

2. **Healing Effects**: Healing is applied immediately when the item is picked up, but is NOT re-applied during recalculation (it's a one-time effect).

3. **Server Authority**: All item creation and validation happens server-side. Clients only request discard, server validates and processes.

4. **Synchronization**: Player stats are already synchronized via `PlayerUpdatePacket` every tick. No special synchronization needed after stat changes.

5. **Performance**: Recalculation is O(n) where n is number of items. This should be fast even with many items since n is typically small (<50).

6. **Item Persistence**: Items are stored in the server's item registry and in each player's inventory. When a player reconnects, they receive a `WorldSnapshot` which includes their inventory.

7. **Memory Management**: Items are stored in an unordered_map and only removed when discarded. Consider adding a cleanup mechanism if item IDs could overflow (unlikely with uint32_t).

8. **UI Thread Safety**: The inventory UI is rendered on the main thread, so no special synchronization needed with `world_state_mutex` already in place.

---

## Future Enhancements (Out of Scope)

- Item trading between players
- Item rarity tiers (common, rare, legendary)
- Item sets with bonus effects
- Item durability/usage limits
- Equipment slots (weapon, armor, accessory)
- Item upgrade/crafting system
- Item visual representation in world (before pickup)
- Item tooltips with detailed stat breakdowns
- Inventory sorting/filtering
- Item icons and custom UI styling

---

## Summary

This implementation adds a complete inventory and item system with:
- ✅ Server-authoritative item drops
- ✅ RNG-based drops on enemy death
- ✅ Stat modification system (flat + percentage)
- ✅ Order-dependent stat calculation
- ✅ Full recalculation on item discard
- ✅ ImGui inventory UI
- ✅ Network synchronization
- ✅ Cereal serialization support

**Estimated Files Changed:** 17 files  
**Estimated New Files:** 5 files  
**Estimated Lines of Code:** ~800-1000 new lines
