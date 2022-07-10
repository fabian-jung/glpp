#pragma once

#include <assimp/material.h>
#include <glm/glm.hpp>
#include <string_view>
#include <vector>

namespace glpp::asset {
enum class op_t {
	addition, // T = T1 + strength*T2
	multiplication, // T = T1*T2
	division, // T= T1/T2
	smooth_addition, // T = (T1 + T2) - (T1 * T2)
	signed_addition // T = T1 + (T2-0.5)
};
	
struct texture_stack_entry_t {
	size_t texture_key;
	float strength;
	op_t op = op_t::addition;
};

using texture_stack_t = std::vector<texture_stack_entry_t>;
struct material_t {
	std::string name;
	glm::vec3 diffuse { 0.0f };
	texture_stack_t diffuse_textures {};
	glm::vec3 emissive { 0.0f };
	texture_stack_t emissive_textures {};
	glm::vec3 ambient { 0.0f };
	texture_stack_t ambient_textures {};
	glm::vec3 specular { 0.0f };
	texture_stack_t specular_textures {};
	float shininess = 6;
};

}

std::ostream& operator<<(std::ostream&,const glpp::asset::material_t&);
std::ostream& operator<<(std::ostream& lhs, const glpp::asset::op_t& rhs);