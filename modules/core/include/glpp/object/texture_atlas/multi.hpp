#pragma once

#include <iterator>
#include "glpp/object/texture.hpp"

namespace glpp::object::texture_atlas {

struct multi_policy_t {
	using key_t = unsigned int;
	using storage_t = std::unordered_map<key_t, texture_t>;
	using slot_storage_t = std::vector<texture_slot_t>;
	
	template <class Image>
	static key_t alloc(storage_t& storage, const Image& image) {
		key_t key = 0;
		while(storage.count(key) > 0) {
			++key;
		}
		storage.insert({key, texture_t{image}});
		return key;
	}

	template <class Image>
	static void update(storage_t& storage, const key_t key, const Image& image) {
		storage.at(key).update(image);
	}
	
	static slot_storage_t slots_from_storage(const storage_t& storage) {
		slot_storage_t result;
		std::transform(
			storage.begin(),
			storage.end(),
			std::inserter(result, result.end()),
			[](const auto& kv) -> slot_storage_t::value_type {
				const auto& [key, texture] = kv;
				auto slot = texture.bind_to_texture_slot();
				return slot;
			}
		);
		return result;
	}
};

} // namespace name
