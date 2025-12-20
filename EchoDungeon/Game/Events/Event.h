#pragma once
#include "Imports/common.h"
#include <future>

/*
* @brief Base class for all event data. Event data is returned to callbacks when an event is triggered.
*/
class BaseEventData {
public:
	virtual ~BaseEventData() = default;
};

/*
* @brief Template class for managing events with specific event data types.
* @tparam EventData The type of data associated with the event, must derive from BaseEventData.
*/
template <typename EventData>
class Event {
	static_assert(std::is_base_of<BaseEventData, EventData>::value, "EventData must derive from BaseEventData");
public:
	using Callback = std::function<void(const EventData&)>; // Data returned to callbacks
	using CallbackID = int; // Unique identifier for each registered callback to allow unregistering

	/*
	* @brief Register a callback for the event.
	* @param callback The callback to register.
	* @return A unique identifier for the registered callback.
	*/
	static CallbackID register_callback(Callback callback) {
		CallbackID callback_id = next_callback_id++;
		callbacks[callback_id] = callback;
		return callback_id;
	}

	/*
	* @brief Check if a callback is registered
	* @param callback_id The unique identifier of the callback to check.
	* @return True if the callback is registered, false otherwise.
	*/
	static bool is_callback_registered(CallbackID callback_id) {
		return callbacks.find(callback_id) != callbacks.end();
	}

	/*
	* @brief Remove a callback from the event, so it is no longer triggered.
	* @param callback_id The unique identifier of the callback to remove.
	*/
	static void unregister_callback(CallbackID callback_id) {
		auto it = callbacks.find(callback_id);
		if (it != callbacks.end()) {
			callbacks.erase(it);
		}
	}

	/*
	* @brief Trigger an event with the given data, calling all registered callbacks asynchronously.
	* @param data The event data to attach
	*/
	static void trigger(const EventData& data) {
		auto callbacks_copy = callbacks; // or just: auto callbacks_copy = callbacks;

		for (const auto& [id, callback] : callbacks_copy) {
			if (callback) {
				std::async(std::launch::async, callback, data);
			}
			else {
				ERROR("Invalid callback with id " + std::to_string(id) + " found.");
				callbacks.erase(id); // Remove invalid callback if it is null
			}
		}

	}

	static CallbackID next_callback_id; // Next available callback ID (incremented on each registration)
	static std::unordered_map<CallbackID, Callback> callbacks; // Map of registered callbacks
};


// Set defaults
template <typename EventData>
typename Event<EventData>::CallbackID Event<EventData>::next_callback_id = 0;

template <typename EventData>
std::unordered_map<typename Event<EventData>::CallbackID, typename Event<EventData>::Callback> Event<EventData>::callbacks;