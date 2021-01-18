#include "glpp/asset/shading/flat.hpp"
#include "glpp/asset/shading/shader_factory.hpp"

namespace glpp::asset::shading {

void unroll_tex_stack(shader_factory_t& factory, const material_t::texture_stack_t& stack) {
	const auto to_string = [](material_t::op_t op) -> std::string {
		switch(op) {
			case material_t::op_t::addition:
				return "+=";
			case material_t::op_t::multiplication:
				return "*=";
			case material_t::op_t::division:
				return "/=";
			default:
				throw std::runtime_error("not implemented");
		}
	};

	std::string tex_stack_handling = "";

	int i = 0;
	for(const auto& t : stack) {
		tex_stack_handling += "FragColor "+to_string(t.op)+" "+std::to_string(t.strength)+"*texture(tex_stack_"+std::to_string(i)+", v_uv);\n";
		factory.set("<tex_stack_declaration>", std::string("uniform sampler2D tex_stack_"+std::to_string(i++)+";\n\t<tex_stack_declaration>"));
	}
	factory.set("<tex_stack_declaration>", std::string_view(""));
	factory.set("<tex_stack_handling>", tex_stack_handling);
}

flat_t::flat_t(texture_storage_t& texture_storage, flat_shading_channel_t source) :
	m_texture_storage(texture_storage),
	m_source(source)
{}

std::string flat_t::vertex_shader_code(const material_t&) const  {
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

std::string flat_t::fragment_shader_code(const material_t& material) const {
	std::string code_template =
	R"(
	#version 450 core
	in vec3 v_world_pos;
	in vec3 v_norm;
	in vec2 v_uv;
	out vec4 FragColor;

	<tex_stack_declaration>

	void main()
	{
		FragColor.xyz = <channel>;

		<tex_stack_handling>
	};
	)";

	shader_factory_t factory(code_template);

	factory.set("<ambient>", material.ambient);
	factory.set("<diffuse>", material.diffuse);
	factory.set("<specular>", material.specular);

	switch(m_source) {
		case flat_shading_channel_t::ambient:
			factory.set("<channel>", material.ambient);
			unroll_tex_stack(factory, material.ambient_textures);
			break;
		case flat_shading_channel_t::diffuse:
			factory.set("<channel>", material.diffuse);
			unroll_tex_stack(factory, material.diffuse_textures);
			break;
		case flat_shading_channel_t::specular:
			factory.set("<channel>", material.specular);
			unroll_tex_stack(factory, material.specular_textures);
			break;
		case flat_shading_channel_t::emission:
			factory.set("<channel>", material.emissive);
			unroll_tex_stack(factory, material.emissive_textures);
			break;
	}

	return factory.code();
}

flat_t::renderer_t flat_t::renderer(const material_t& material) const {

	renderer_t result(
		object::shader_t(object::shader_type_t::vertex, vertex_shader_code(material)),
		object::shader_t(object::shader_type_t::fragment, fragment_shader_code(material))
	);
	result.set_uniform_name(&uniform_description_t::model_matrix, "model_matrix");
	result.set_uniform_name(&uniform_description_t::view_projection, "view_projection");

	return result;
}

void flat_t::set_up(renderer_t& renderer, const material_t& material) const {
	int i = 0;
	const auto& tex_stack = [&](){
		switch(m_source) {
			case flat_shading_channel_t::ambient:
				return material.ambient_textures;
			case flat_shading_channel_t::diffuse:
				return material.diffuse_textures;
			case flat_shading_channel_t::specular:
				return material.specular_textures;
			case flat_shading_channel_t::emission:
				return material.emissive_textures;
		}
		throw std::runtime_error("Trying to use shading channel, that is not implemented.");
	}();
	for(const auto& t : tex_stack) {
		const auto& slot = m_texture_storage.slot(t.file);
		renderer.set_texture(("tex_stack_"+std::to_string(i++)).c_str(), slot);
	}
}

}
