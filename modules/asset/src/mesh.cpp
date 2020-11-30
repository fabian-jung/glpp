#include <glpp/asset/mesh.hpp>

namespace glpp::asset {

mesh_t::mesh_t(const aiMesh* mesh) noexcept :
	m_mesh(mesh)
{}

[[nodiscard]] mesh_t::model_t mesh_t::model() const {
	model_t model;
	model.verticies.reserve(m_mesh->mNumVertices);
	const auto& vecticies = m_mesh->mVertices;
	const auto& tex = m_mesh->mTextureCoords[0];
	const auto& norm = m_mesh->mNormals;

	for(auto j = 0u; j < m_mesh->mNumVertices; ++j) {
		model.verticies.emplace_back(vertex_description_t{
			glm::vec3{ vecticies[j].x,  vecticies[j].y,  vecticies[j].z},
			glm::vec3{ norm[j].x, norm[j].y, norm[j].z },
			glm::vec2{ tex[j].x, tex[j].y }
		});
	}

	std::for_each_n(m_mesh->mFaces, m_mesh->mNumFaces, [&](const auto& face){
		std::copy_n(face.mIndices, face.mNumIndices, std::back_inserter(model.indicies));
	});

	return model;
}

}
