#include "glpp/asset/render/scene_view.hpp"
#include "glpp/asset/render/scene_renderer.hpp"

namespace glpp::asset::render {


scene_view_t::scene_view_t(const scene_t& scene) :
	m_meshes(scene.materials.size())
{
	for(const auto& mesh : scene.meshes) {
		m_meshes[mesh.material_index].emplace_back(mesh);
	}
}

const std::vector<mesh_view_t>& scene_view_t::meshes_by_material(material_index_t index) const {
	return m_meshes[index];
}

scene_view_t::material_index_t scene_view_t::materials() const {
	return m_meshes.size();
}

}
