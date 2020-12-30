#pragma once

#include <variant>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include "scene_graph_t.hpp"
#include <glpp/render/camera.hpp>

namespace glpp::asset {

using any_light_t = std::variant<ambient_light_t, directional_light_t, point_light_t, spot_light_t>;

class importer_t {
public:
	importer_t(const std::string& file);

	std::vector<mesh_t> meshes() const;

	std::vector<material_t> materials() const;

	[[deprecated]]
	std::vector<material_t> materials(std::ostream& logger) const;

	std::vector<render::camera_t> cameras() const;

	template <class T>
	std::vector<T> lights();

	std::vector<any_light_t> all_lights() const;

private:
	Assimp::Importer m_importer;
	const aiScene* m_scene;
	scene_graph_t m_scene_graph;
};

}
