#pragma once

#include <variant>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "mesh.hpp"
#include "material.hpp"
#include "light.hpp"
#include <glpp/render/camera.hpp>

namespace glpp::asset {

using any_light_t = std::variant<ambient_light_t, directional_light_t, point_light_t, spot_light_t>;

class importer_t {
public:
	using material_map_t = std::unordered_map<std::string, material_t>;
	enum class material_policy_t {
		augment, // use materials from file as default and overwrite materials with the material_map
		replace // dont use any materials imported from the file
	};
	importer_t(const std::string& file, material_map_t material_map = {}, material_policy_t material_policy = material_policy_t::augment);

	std::vector<mesh_t> meshes() const;

	std::vector<material_t> materials() const;

	[[deprecated]]
	std::vector<material_t> materials(std::ostream& logger) const;

	std::vector<render::camera_t> cameras() const;

	template <class T>
	std::vector<T> lights() const;

	std::vector<any_light_t> all_lights() const;

private:
	Assimp::Importer m_importer;
	const aiScene* m_scene;
	material_map_t m_material_map;
	material_policy_t m_material_policy;
};

}
