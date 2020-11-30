#include "glpp/asset/shading/normal.hpp"

namespace glpp::asset::shading {

std::string normal_t::vertex_shader_code(const material_t&) const {
	return vertex_shader_code();
}

std::string normal_t::fragment_shader_code(const material_t&) const {
	return fragment_shader_code();
}

normal_t::renderer_t normal_t::renderer(const material_t&) const {
	return renderer();
}

std::string normal_t::vertex_shader_code() const {
	return R"(
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
		v_uv = uv;
	};
	)";
	
}

std::string normal_t::fragment_shader_code() const {
	return R"(
	#version 450 core
	in vec3 v_world_pos;
	in vec3 v_norm;
	in vec2 v_uv;
	out vec4 FragColor;

	void main()
	{
		FragColor.xyz =0.5*(1+v_norm);		
	};
	)";
}

void normal_t::set_up(renderer_t&, const material_t&) const
{}

normal_t::renderer_t normal_t::renderer() const {
	renderer_t result(
		core::object::shader_t(core::object::shader_type_t::vertex, vertex_shader_code()),
		core::object::shader_t(core::object::shader_type_t::fragment, fragment_shader_code())
	);
	result.set_uniform_name(&uniform_description_t::model_matrix, "model_matrix");
	result.set_uniform_name(&uniform_description_t::view_projection, "view_projection");
	
	return result;
}

}
