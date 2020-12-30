#pragma once

#include <assimp/scene.h>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace glpp::asset {

struct scene_graph_t {
	explicit scene_graph_t(const aiScene* base, const aiNode*);

	[[deprecated]]
	scene_graph_t(const aiScene* base, const aiNode*, std::ostream& logger);

	std::unordered_map<std::string, const glm::mat4> scene;

private:
	void step(const aiScene* base, const aiNode* node, glm::mat4 old);
};

}
