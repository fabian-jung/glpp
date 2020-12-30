#include <glpp/asset/scene_graph_t.hpp>


#include <iostream>
#include <iterator>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace glpp::asset {

template <class Functor>
void step_scene_graph(const aiNode* node, const Functor& fn) {
	fn(node);

	std::for_each_n(node->mChildren, node->mNumChildren, [&fn](const aiNode* child){
		step_scene_graph(child, fn);
	});
}


std::ostream& operator<<(std::ostream& lhs, const aiNode& node) {

	step_scene_graph(&node, [&](const aiNode* node) {
		const auto  transformation = glm::make_mat4(&(node->mTransformation.a1));
		lhs << "node " << node->mName.C_Str() << ":" << glm::to_string(transformation) << "\n";
		if(node->mMetaData) {
			const auto* keys = node->mMetaData->mKeys;
			const auto* entries = node->mMetaData->mValues;
			for(auto i = 0u; i < node->mMetaData->mNumProperties; ++i) {
				const auto& k = keys[i];
				const auto& v = entries[i];
				lhs << "	" << k.C_Str() << ":";
				switch(v.mType) {
					case AI_BOOL:
						lhs << *reinterpret_cast<bool*>(v.mData);
						break;
					case AI_INT32:
						lhs << *reinterpret_cast<std::int32_t*>(v.mData);
						break;
					case AI_UINT64:
						lhs << *reinterpret_cast<std::int64_t*>(v.mData);
						break;
					case AI_FLOAT:
						lhs << *reinterpret_cast<float*>(v.mData);
						break;
					case AI_DOUBLE:
						lhs << *reinterpret_cast<double*>(v.mData);
						break;
					case AI_AISTRING:
						lhs << reinterpret_cast<aiString*>(v.mData)->C_Str();
						break;
					case AI_AIVECTOR3D: {
						const auto* vec = reinterpret_cast<aiVector3D*>(v.mData);
						lhs << "{ " << vec->x << ", " << vec->y << ", " << vec->z << " }";
						break;
					}
					default:
						lhs << "<unprintable>";
				}
				lhs << "\n";
			}
		}

	});

	return lhs;
}


void scene_graph_t::step(const aiScene* base, const aiNode* node, glm::mat4 old) {
	std::string key(node->mName.C_Str());
	glm::mat4 transformation = glm::transpose(glm::make_mat4(&(node->mTransformation.a1)))*old;
	scene.emplace(
		std::move(key),
		transformation
	);
	std::for_each_n(node->mMeshes, node->mNumMeshes, [&](unsigned int mesh) {
		std::string key = base->mMeshes[mesh]->mName.C_Str();
		scene.emplace(
			std::move(key),
					  transformation
		);
	});

	std::for_each_n(node->mChildren, node->mNumChildren, [&](const aiNode* next){
		step(base, next, transformation);
	});
}

scene_graph_t::scene_graph_t(const aiScene* base, const aiNode* root) {
	step(base, root, glm::mat4(1.0f));
}

scene_graph_t::scene_graph_t(const aiScene* base, const aiNode* root, std::ostream& logger) :
	scene_graph_t(base, root)
{
	logger << *root << std::endl;
}

}
