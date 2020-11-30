#pragma once

#include "glpp/asset/material.hpp"
#include "glpp/asset/mesh.hpp"

namespace glpp::asset::shading {

class depth_t {
public:
	
	struct uniform_description_t {
		glm::mat4 model_matrix;
		glm::mat4 view_projection;
	};
	
	using vertex_description_t = glpp::asset::mesh_t::vertex_description_t;
	using model_t = mesh_t::model_t;
	using view_t = core::render::view_t<model_t>;
	using renderer_t = core::render::renderer_t<uniform_description_t>;
	
	explicit depth_t() = default;

	std::string vertex_shader_code() const;
	std::string fragment_shader_code() const;
	
	std::string vertex_shader_code(const material_t& material) const;
	std::string fragment_shader_code(const material_t& material) const;
	
	void set_up(renderer_t&, const material_t&) const;

	renderer_t renderer() const;
	renderer_t renderer(const material_t& material) const;
};

}
