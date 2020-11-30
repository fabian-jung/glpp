#pragma once

#include "glpp/asset/scene.hpp"
#include "mesh_view.hpp"

namespace glpp::asset::render {

class scene_view_t {
public:
	using material_index_t = size_t;

	explicit scene_view_t(const scene_t& scene);

	const std::vector<mesh_view_t>& meshes_by_material(material_index_t index) const;
	material_index_t materials() const;

private:
	std::vector<std::vector<mesh_view_t>> m_meshes;
};

}