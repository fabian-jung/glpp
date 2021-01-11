#pragma once

#include <assimp/material.h>
#include <glm/glm.hpp>

namespace glpp::asset {

struct material_t {
	enum class op_t {
		addition, // T = T1 + strength*T2
		multiplication, // T = T1*T2
		division, // T= T1/T2
		smooth_addition, // T = (T1 + T2) - (T1 * T2) 
		signed_addition // T = T1 + (T2-0.5) 
	};
		
	struct texture_stack_entry_t {
		float strength;
		std::string file;
		op_t op = op_t::addition;
	};
	using texture_stack_t = std::vector<texture_stack_entry_t>;

	explicit material_t(const aiMaterial* material);
	material_t(const aiMaterial* material, std::ostream& logger);

	const std::string name;
	const glm::vec3 diffuse;
	const texture_stack_t diffuse_textures;
	const glm::vec3 emissive;
	const texture_stack_t emissive_textures;
	const glm::vec3 ambient;
	const texture_stack_t ambient_textures;
	const glm::vec3 specular;
	const texture_stack_t specular_textures;
	float shininess = 6;
};

}

std::ostream& operator<<(std::ostream&,const glpp::asset::material_t&);
std::ostream& operator<<(std::ostream& lhs, const glpp::asset::material_t::op_t& rhs);
