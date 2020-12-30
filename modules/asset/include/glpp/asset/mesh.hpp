#pragma once

#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glpp/core.hpp>

namespace glpp::asset {

class mesh_t {
public:
	struct vertex_description_t {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex;
	};

	using model_t = glpp::render::indexed_model_t<vertex_description_t>;

	explicit mesh_t(glm::mat4 model_matrix, const aiMesh* mesh);

	glm::mat4 model_matrix;
	model_t model;
};

}
