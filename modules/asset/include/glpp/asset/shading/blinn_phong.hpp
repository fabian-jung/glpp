#pragma once

#include "glpp/asset/material.hpp"
#include "glpp/asset/light.hpp"
#include "glpp/asset/mesh.hpp"
#include "glpp/asset/importer.hpp"
#include "glpp/asset/shading/texture_storage.hpp"

namespace glpp::asset::shading {

class blinn_phong_t {
public:
// 	template <template <class> class Container>
	explicit blinn_phong_t(const std::vector<point_light_t>& point_lights) :
		m_point_lights(point_lights.begin(), point_lights.end())
	{}

	explicit blinn_phong_t(const importer_t& importer, shading::texture_storage_t& texture_storage) :
		blinn_phong_t(importer.lights<point_light_t>())
	{}

	struct uniform_description_t {
		glm::mat4 model_matrix;
		glm::mat4 view_projection;

		glm::vec3 point_light_position;
		glm::vec3 point_light_ambient;
		glm::vec3 point_light_diffuse;
		glm::vec3 point_light_specular;
		float point_light_attenuation_constant;
		float point_light_attenuation_linear;
		float point_light_attenuation_quadratic;
	};

	using vertex_description_t = glpp::asset::mesh_t::vertex_description_t;
	using model_t = glpp::asset::mesh_t::model_t;
	using view_t = glpp::render::view_t<model_t>;
	using renderer_t = render::renderer_t<uniform_description_t>;

	void set_up(renderer_t& renderer, const material_t& material) const;
	renderer_t renderer(const material_t& material) const;

private:
	std::string assemble_fragment_shader(const material_t& material) const;

	std::vector<point_light_t> m_point_lights;
};

}
