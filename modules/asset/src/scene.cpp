#include "glpp/asset/scene.hpp"


namespace glpp::asset {

scene_t::scene_t(const importer_t& importer) :
	meshes(importer.meshes()),
	materials(importer.materials()),
	ambient_lights(importer.lights<ambient_light_t>()),
	directional_lights(importer.lights<directional_light_t>()),
	point_lights(importer.lights<point_light_t>()),
	spot_lights(importer.lights<spot_light_t>())
{}

scene_t::scene_t(
	std::vector<mesh_t> meshes,
	std::vector<material_t> materials,
	std::vector<ambient_light_t> ambient_lights,
	std::vector<directional_light_t> directional_lights,
	std::vector<point_light_t> point_lights,
	std::vector<spot_light_t> spot_lights
) :
	meshes(std::move(meshes)),
	materials(std::move(materials)),
	ambient_lights(std::move(ambient_lights)),
	directional_lights(std::move(directional_lights)),
	point_lights(std::move(point_lights)),
	spot_lights(std::move(spot_lights))
{}

}
