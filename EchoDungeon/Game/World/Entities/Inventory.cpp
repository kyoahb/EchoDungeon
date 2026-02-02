#include "Inventory.h"
#include <algorithm>

void Inventory::add_item(uint32_t item_id) {
	item_ids.push_back(item_id);
	INFO("Added item " + std::to_string(item_id) + " to inventory");
}

void Inventory::remove_item(uint32_t item_id) {
	auto it = std::find(item_ids.begin(), item_ids.end(), item_id);
	if (it != item_ids.end()) {
		item_ids.erase(it);
		INFO("Removed item " + std::to_string(item_id) + " from inventory");
	}
}

bool Inventory::has_item(uint32_t item_id) const {
	return std::find(item_ids.begin(), item_ids.end(), item_id) != item_ids.end();
}

const std::vector<uint32_t>& Inventory::get_item_ids() const {
	return item_ids;
}
