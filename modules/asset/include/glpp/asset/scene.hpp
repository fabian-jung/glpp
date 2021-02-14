#pragma once

#include <type_traits>
#include "mesh.hpp"
#include "light.hpp"
#include "importer.hpp"

namespace glpp::asset {

struct scene_t {
public:



	explicit scene_t(const importer_t& importer);

	std::vector<mesh_t> meshes;
	std::vector<material_t> materials;

	std::vector<ambient_light_t> ambient_lights;
	std::vector<directional_light_t> directional_lights;
	std::vector<point_light_t> point_lights;
	std::vector<spot_light_t> spot_lights;

};

};
