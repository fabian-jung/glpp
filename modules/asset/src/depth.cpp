#include "glpp/asset/shading/depth.hpp"

namespace glpp::asset::shading {

std::string depth_t::vertex_shader_code(const material_t&) const {
	return vertex_shader_code();
}

std::string depth_t::fragment_shader_code(const material_t&) const {
	return fragment_shader_code();
}

depth_t::renderer_t depth_t::renderer(const material_t&) const {
	return renderer();
}

std::string depth_t::vertex_shader_code() const {
	return R"(
	#version 450 core
	layout (location = 0) in vec3 pos;
	layout (location = 1) in vec3 norm;
	layout (location = 2) in vec2 uv;
		
	uniform mat4 model_matrix;
	uniform mat4 view_projection;
	
	void main()
	{
		vec3 v_world_pos = (model_matrix*vec4(pos, 1.0)).xyz;
		gl_Position = view_projection*vec4(v_world_pos, 1.0);
	};
	)";
	
}

std::string depth_t::fragment_shader_code() const {
	return R"(
	#version 450 core
		
	in float v_depth;
	
	out vec4 FragColor;

	void main()
	{
		FragColor.xyz = gl_FragCoord.zzz;
	};
	)";
}

void depth_t::set_up(renderer_t&, const material_t&) const {
}

depth_t::renderer_t depth_t::renderer() const {
	renderer_t result(
		core::object::shader_t(core::object::shader_type_t::vertex, vertex_shader_code()),
		core::object::shader_t(core::object::shader_type_t::fragment, fragment_shader_code())
	);
	result.set_uniform_name(&uniform_description_t::model_matrix, "model_matrix");
	result.set_uniform_name(&uniform_description_t::view_projection, "view_projection");
	
	return result;
}

}
