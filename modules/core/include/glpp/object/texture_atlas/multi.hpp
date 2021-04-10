#pragma once

#include <iterator>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "glpp/object/texture.hpp"

namespace glpp::object::texture_atlas {

struct multi_policy_t {
	using key_t = size_t;
	using storage_t = std::vector<texture_t>;
	using slot_storage_t = std::vector<texture_slot_t>;
	
	template <class Image>
	static key_t alloc(storage_t& storage, const Image& image) {
		const key_t key = storage.size();
		storage.emplace_back(texture_t{image});
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
			std::back_inserter(result),
			[](const texture_t& tex) {
				return tex.bind_to_texture_slot();
			}
		);
		return result;
	}
};

} // namespace name
