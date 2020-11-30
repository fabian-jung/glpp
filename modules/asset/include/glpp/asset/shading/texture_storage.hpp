#pragma once

#include <memory>
#include <unordered_map>

#include "glpp/object/texture.hpp"

namespace glpp::asset::shading {

class texture_storage_t {
public:
	object::texture_t& texture(const std::string& file);
	const object::texture_slot_t& slot(const std::string& file);

public:
	struct texture_storage_element_t {
		object::texture_t texture;
		object::texture_slot_t slot;
	};
	std::unordered_map<std::string, texture_storage_element_t> m_textures;

	texture_storage_element_t& get_storage_element(const std::string& file);
};
}
