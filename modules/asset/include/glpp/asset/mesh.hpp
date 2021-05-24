#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glpp/core.hpp>
#include "material.hpp"

namespace glpp::asset {

struct mesh_t {
	struct vertex_description_t {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex;
	};

	using model_t = glpp::core::render::indexed_model_t<vertex_description_t>;

	model_t model {};
	glm::mat4 model_matrix { 1.0f };
	unsigned int material_index { 0 };
};

}