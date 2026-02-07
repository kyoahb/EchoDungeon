#include "LevelGenerator.h"

void LevelGenerator::generate_level(ServerWorldManager& manager) {
	raylib::Color obstacle_color = { 110, 110, 110, 250 };
	// Slightly transparent blue means enemies moving through walls
	//	will be visible 
	
	// ===== BORDER WALLS =====
	// Left wall
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -25.5f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 25.0f },
		obstacle_color);
	// Right wall
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 25.5f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 25.0f },
		obstacle_color);
	// Top wall
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 0.0f, 1.0f, -25.5f }, { 0.0f, 0.0f, 0.0f }, { 25.0f, 0.5f, 0.5f },
		obstacle_color);
	// Bottom wall
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 0.0f, 1.0f, 25.5f }, { 0.0f, 0.0f, 0.0f }, { 25.0f, 0.5f, 0.5f },
		obstacle_color);

	// (A) Top-left L-shape
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -19.0f, 1.0f, -21.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -21.0f, 1.0f, -19.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (B) Top-center T-shape
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -5.5f, 1.0f, -21.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical stem
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -6.0f, 1.0f, -19.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Stem base extension
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -6.5f, 1.0f, -16.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.5f, 0.5f },
		obstacle_color);

	// (C) Single square near top-right
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 16.0f, 1.0f, -23.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (D) Top-right structure
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 15.5f, 1.0f, -21.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 12.0f, 1.0f, -23.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 10.0f, 1.0f, -23.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (E) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -13.0f, 1.0f, -19.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (F) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 7.0f, 1.0f, -18.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (G) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 15.0f, 1.0f, -18.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (H) 4x4 block
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -1.5f, 1.0f, -15.5f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 0.5f, 2.0f },
		obstacle_color);

	// (I) Left enclosure
	// Top horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -20.5f, 1.0f, -15.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 0.5f, 0.5f },
		obstacle_color);
	// Right vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -15.0f, 1.0f, -11.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 4.0f },
		obstacle_color);
	// Single square inside
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -17.0f, 1.0f, -12.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (J) Center-left structure
	// Short horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -10.5f, 1.0f, -15.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -10.0f, 1.0f, -13.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Long horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -5.0f, 1.0f, -11.0f }, { 0.0f, 0.0f, 0.0f }, { 5.5f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -1.0f, 1.0f, -8.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 3.0f },
		obstacle_color);

	// (K) P-shape right side
	// Top horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 12.0f, 1.0f, -12.0f }, { 0.0f, 0.0f, 0.0f }, { 5.5f, 0.5f, 0.5f },
		obstacle_color);
	// Left vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 18.0f, 1.0f, -9.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Right vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 22.0f, 1.0f, -9.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.0f },
		obstacle_color);
	// Bottom horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 14.5f, 1.0f, -8.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 0.5f, 0.5f },
		obstacle_color);
	// Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 12.0f, 1.0f, -9.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (L) Right border horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 23.5f, 1.0f, -7.0f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 0.5f, 0.5f },
		obstacle_color);

	// (M) 4x4 block
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 7.5f, 1.0f, -5.5f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 0.5f, 2.0f },
		obstacle_color);

	// (N) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -15.0f, 1.0f, -5.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (O) Scattered singles
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -12.0f, 1.0f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -7.0f, 1.0f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 2.0f, 1.0f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 19.0f, 1.0f, -4.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (P) Left side vertical + horizontal
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -22.0f, 1.0f, -1.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.0f },
		obstacle_color);
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -20.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 3.5f, 0.5f, 0.5f },
		obstacle_color);

	// (Q) Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -12.0f, 1.0f, -1.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.0f },
		obstacle_color);

	// (R) Right side L-shape
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 18.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 21.0f, 1.0f, 2.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);

	// (S) Horizontal bar + vertical bar
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -5.0f, 1.0f, 3.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -3.0f, 1.0f, 5.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 3.0f },
		obstacle_color);

	// (T) 4x4 block
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -12.5f, 1.0f, 5.5f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 0.5f, 2.0f },
		obstacle_color);

	// (U) Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 8.0f, 1.0f, 6.5f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (V) Right side L-shape
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 19.0f, 1.0f, 9.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 17.0f, 1.0f, 7.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);

	// (W) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 13.0f, 1.0f, 9.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (X) Long horizontal bar + vertical bar
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -11.0f, 1.0f, 11.0f }, { 0.0f, 0.0f, 0.0f }, { 5.5f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -6.0f, 1.0f, 13.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (Y) Horizontal bar + long vertical bar
	// Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 4.5f, 1.0f, 13.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 7.0f, 1.0f, 15.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (Z) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -9.0f, 1.0f, 14.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (AA) Bottom-left structure
	// Short horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -21.5f, 1.0f, 15.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.5f, 0.5f },
		obstacle_color);
	// Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -21.0f, 1.0f, 17.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (BB) Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -12.0f, 1.0f, 16.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);

	// (CC) Vertical bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 2.0f, 1.0f, 19.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 2.5f },
		obstacle_color);

	// (DD) Single square
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -14.0f, 1.0f, 18.0f }, { 0.0f, 0.0f, 0.0f }, { 0.5f, 0.5f, 0.5f },
		obstacle_color);

	// (EE) Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ -1.5f, 1.0f, 19.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 0.5f, 0.5f },
		obstacle_color);

	// (FF) Horizontal bar
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 12.0f, 1.0f, 19.0f }, { 0.0f, 0.0f, 0.0f }, { 2.5f, 0.5f, 0.5f },
		obstacle_color);

	// (GG) 4x4 block
	manager.spawn_object(ObjectType::MODEL, "cube",
		{ 19.5f, 1.0f, 19.5f }, { 0.0f, 0.0f, 0.0f }, { 2.0f, 0.5f, 2.0f },
		obstacle_color);
}
