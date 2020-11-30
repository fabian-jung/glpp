#include <glpp/asset/mesh.hpp>

namespace glpp::asset {

// [[nodiscard]] mesh_t::model_t to_model(const aiMesh* mesh) {
// 	mesh_t::model_t model;
// 	model.verticies.reserve(mesh->mNumVertices);
// 	const auto& vecticies = mesh->mVertices;
// 	const auto& tex = mesh->mTextureCoords[0];
// 	const auto& norm = mesh->mNormals;

// 	for(auto j = 0u; j < mesh->mNumVertices; ++j) {
// 		model.verticies.emplace_back(
// 			mesh_t::vertex_description_t{
// 				glm::vec3{ vecticies[j].x,  vecticies[j].y,  vecticies[j].z},
// 				glm::vec3{ norm[j].x, norm[j].y, norm[j].z },
// 				glm::vec2{ tex[j].x, tex[j].y }
// 			}
// 		);
// 	}

// 	std::for_each_n(mesh->mFaces, mesh->mNumFaces, [&](const auto& face){
// 		std::copy_n(face.mIndices, face.mNumIndices, std::back_inserter(model.indicies));
// 	});

// 	return model;
// }

// mesh_t::mesh_t(glm::mat4 model_matrix, const aiMesh* mesh) :
// 	model_matrix(std::move(model_matrix)),
// 	model(to_model(mesh)),
// 	material_index(mesh->mMaterialIndex)
// {}

}
