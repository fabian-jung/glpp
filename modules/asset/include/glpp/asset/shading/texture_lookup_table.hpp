#pragma once

#include <memory>
#include <unordered_map>

#include <glpp/object/texture.hpp>

namespace glpp::asset::shading {

class texture_lookup_table_t {
	using texture_t = object::texture_t;
	std::shared_ptr<texture_t>& get(const std::string& file);
	
public:
	std::unordered_map<std::string, std::weak_ptr<texture_t>> m_textures;
	
};	
}
