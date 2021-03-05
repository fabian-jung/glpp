#pragma once

#include "glpp/asset/material.hpp"
#include "glpp/asset/mesh.hpp"
#include "glpp/asset/shading/texture_storage.hpp"

namespace glpp::asset::shading {

enum class flat_shading_channel_t {
	ambient,
	diffuse,
	specular,
	emission
};

class flat_t {
public:

	explicit flat_t(texture_storage_t& texture_storage, flat_shading_channel_t source = flat_shading_channel_t::diffuse);

	struct uniform_description_t {
		glm::mat4 model_matrix;
		glm::mat4 view_projection;
	};

	using vertex_description_t = asset::mesh_t::vertex_description_t;
	using model_t = asset::mesh_t::model_t;
	using view_t = render::view_t<model_t>;
	using renderer_t = render::renderer_t<uniform_description_t>;


	std::string vertex_shader_code(const material_t& material) const;
	std::string fragment_shader_code(const material_t& material) const;

	void set_up(renderer_t& renderer, const material_t& material) const;

private:
	renderer_t renderer(const material_t& material) const;
	texture_storage_t& m_texture_storage;
	flat_shading_channel_t m_source;
};

}