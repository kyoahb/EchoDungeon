#pragma once

#include "Imports/common.h"
#include "Game/World/Entities/Item.h"

class Inventory {
public:
	Inventory() = default;
	Inventory(std::vector<uint32_t> initial_items) : item_ids(initial_items) {}
	std::vector<uint32_t> item_ids;

	void add_item(uint32_t item_id);
	void remove_item(uint32_t item_id);
	void draw_ui();
};
