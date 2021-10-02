#pragma once

#include "glpp/asset/material.hpp"
#include "glpp/asset/mesh.hpp"
#include "glpp/core/object/texture_atlas.hpp"
#include "glpp/core/object/shader_factory.hpp"

namespace glpp::asset::shading {

enum class flat_shading_channel_t {
	ambient,
	diffuse,
	specular,
	emission
};

template <class AllocPolicy = core::object::texture_atlas::multi_policy_t>
class flat_t {
public:

	flat_t(const core::object::texture_atlas_t<AllocPolicy>& textures, flat_shading_channel_t source = flat_shading_channel_t::diffuse);

	struct uniform_description_t {
		glm::mat4 model_matrix;
		glm::mat4 view_projection;
	};

	using vertex_description_t = asset::mesh_t::vertex_description_t;
	using model_t = asset::mesh_t::model_t;
	using view_t = core::render::view_t<model_t>;
	using renderer_t = core::render::renderer_t<uniform_description_t>;


	std::string vertex_shader_code(const material_t& material) const;
	std::string fragment_shader_code(const material_t& material) const;

	void set_up(renderer_t& renderer, const core::object::texture_atlas_slot_t<AllocPolicy>& texture_slots) const;
	renderer_t renderer(const material_t& material) const;

private:
	const core::object::texture_atlas_t<AllocPolicy>&  m_textures;
	flat_shading_channel_t m_source;
};

template <class AllocPolicy>
flat_t(const core::object::texture_atlas_t<AllocPolicy>&, flat_shading_channel_t) -> flat_t<AllocPolicy>;

template <class AllocPolicy>
flat_t<AllocPolicy>::flat_t(const core::object::texture_atlas_t<AllocPolicy>& textures, flat_shading_channel_t source) :
	m_textures(textures),
	m_source(source)
{}

template <class AllocPolicy>
std::string flat_t<AllocPolicy>::vertex_shader_code(const material_t&) const  {
	constexpr auto code =
	R"(
	#version 450 core
	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec3 norm;
	layout (location = 2) in vec2 uv;

	out vec3 v_world_pos;
	out vec3 v_norm;
	out vec2 v_uv;

	uniform mat4 model_matrix;
	uniform mat4 view_projection;

	void main()
	{
		v_world_pos = (model_matrix*vec4(pos, 1.0)).xyz;
		gl_Position = view_projection*vec4(v_world_pos, 1.0);
		v_norm = normalize(model_matrix*vec4(norm,1)-(model_matrix*vec4(0,0,0,1))).xyz;
		v_uv = vec2(uv.x, 1-uv.y);
	};
	)";
	return code;
}

namespace detail {
template <class AllocPolicy>
void unroll_tex_stack(core::object::shader_factory_t& factory, const texture_stack_t& stack, const core::object::texture_atlas_t<AllocPolicy>& textures) {
	const auto to_string = [](op_t op) -> std::string {
		switch(op) {
			case op_t::addition:
				return "+=";
			case op_t::multiplication:
				return "*=";
			case op_t::division:
				return "/=";
			default:
				throw std::runtime_error("not implemented");
		}
	};

	factory.set("<tex_stack_declaration>", textures.declaration("textures"));

	std::string tex_stack_handling = "";
	for(const auto& t : stack) {
		tex_stack_handling += "FragColor "+to_string(t.op)+" "+std::to_string(t.strength)+"*"+textures.get(t.texture_key).fetch("textures", "v_uv")+";\n";
	}

	factory.set("<tex_stack_handling>", tex_stack_handling);
}
}

template <class AllocPolicy>
std::string flat_t<AllocPolicy>::fragment_shader_code(const material_t& material) const {
	std::string code_template =
	R"(
	#version 450 core
	in vec3 v_world_pos;
	in vec3 v_norm;
	in vec2 v_uv;
	out vec4 FragColor;

	<tex_stack_declaration>;

	void main()
	{
		FragColor.xyz = <channel>;

		<tex_stack_handling>
	};
	)";

	core::object::shader_factory_t factory(code_template);

	switch(m_source) {
		case flat_shading_channel_t::ambient:
			factory.set("<channel>", material.ambient);
			detail::unroll_tex_stack(factory, material.ambient_textures, m_textures);
			break;
		case flat_shading_channel_t::diffuse:
			factory.set("<channel>", material.diffuse);
			detail::unroll_tex_stack(factory, material.diffuse_textures, m_textures);
			break;
		case flat_shading_channel_t::specular:
			factory.set("<channel>", material.specular);
			detail::unroll_tex_stack(factory, material.specular_textures, m_textures);
			break;
		case flat_shading_channel_t::emission:
			factory.set("<channel>", material.emissive);
			detail::unroll_tex_stack(factory, material.emissive_textures, m_textures);
			break;
	}

	return factory.code();
}

template <class AllocPolicy>
typename flat_t<AllocPolicy>::renderer_t flat_t<AllocPolicy>::renderer(const material_t& material) const {
	renderer_t result(
		core::object::shader_t(core::object::shader_type_t::vertex, vertex_shader_code(material)),
		core::object::shader_t(core::object::shader_type_t::fragment, fragment_shader_code(material))
	);
	result.set_uniform_name(&uniform_description_t::model_matrix, "model_matrix");
	result.set_uniform_name(&uniform_description_t::view_projection, "view_projection");

	return result;
}

template <class AllocPolicy>
void flat_t<AllocPolicy>::set_up(renderer_t& renderer, const core::object::texture_atlas_slot_t<AllocPolicy>& texture_slots) const {
	renderer.set_texture_atlas("textures", texture_slots);
}

extern template class flat_t<core::object::texture_atlas::multi_policy_t>;
extern template class flat_t<core::object::texture_atlas::grid_policy_t>;
}