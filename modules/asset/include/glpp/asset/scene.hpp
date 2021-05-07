#pragma once

#include "mesh.hpp"
#include "light.hpp"

namespace glpp::asset {

struct scene_t {

	scene_t() = default;
	explicit scene_t(const char* file_name);

	scene_t(scene_t&&) = default;
	scene_t(const scene_t&) = default;
	
	scene_t& operator=(scene_t&&) = default;
	scene_t& operator=(const scene_t&) = default;
	
	std::vector<std::string> textures;
	std::vector<material_t> materials;
	std::vector<mesh_t> meshes;

	std::vector<ambient_light_t> ambient_lights;
	std::vector<directional_light_t> directional_lights;
	std::vector<point_light_t> point_lights;
	std::vector<spot_light_t> spot_lights;

};

}