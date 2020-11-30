#include "glpp/asset/shading/blinn_phong.hpp"
#include "glpp/object/shader_factory.hpp"

namespace glpp::asset::shading {

constexpr auto vertex_shader_code = R"(
	#version 330 core
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


#warning using LN for specular highlight is wrong, it shoud be LN with  respect to the eye
constexpr auto fragment_shader_code = R"(
	#version 330 core

	in vec3 v_world_pos;
	in vec3 v_norm;
	in vec2 v_uv;
	out vec4 FragColor;

	uniform vec3 point_light_position[<point_light_size>];
	uniform vec3 point_light_ambient[<point_light_size>];
	uniform vec3 point_light_diffuse[<point_light_size>];
	uniform vec3 point_light_specular[<point_light_size>];
	uniform float point_light_attenuation_constant[<point_light_size>];
	uniform float point_light_attenuation_linear[<point_light_size>];
	uniform float point_light_attenuation_quadratic[<point_light_size>];

	void main()
	{
		vec3 ambient = <ambient>;
		vec3 diffuse = <diffuse>;
		vec3 specular = <specular>;

		FragColor = vec4(0,0,0,1);
		for(int i = 0; i < <point_light_size>; ++i) {
// 			FragColor.xyz += diffuse;
			float dis = distance(v_world_pos, point_light_position[i]);
			float LN = max(dot(v_norm,normalize(point_light_position[i]-v_world_pos)), 0.0);
			float attenuation =
				1/(
// 					/*point_light_attenuation_constant[i]+*/1+
// 					/*point_light_attenuation_linear[i]**/dis+
// 					point_light_attenuation_quadratic[i]*dis*dis
					1+
					dis+
					pow(/*point_light_attenuation_quadratic[i]**/dis, 2)
				);
			float Is = pow(LN, <shininess>);
// 			FragColor.xyz += point_light_ambient[i]*ambient;
			FragColor.xyz += 15*/*point_light_diffuse[i]**/diffuse*(LN*attenuation);
			FragColor.xyz += /*point_light_specular[i]**/specular*Is*attenuation;
		}
	}
)";


template <class T, class U>
auto component_as_vector(std::vector<U> container, T U::* member) {
	std::vector<T> result;
	std::transform(container.begin(), container.end(), std::back_inserter(result), [member](const auto compound){ return compound.*member; });
	return result;
}

blinn_phong_t::renderer_t blinn_phong_t::renderer(const material_t& material) const {
	shader_factory_t fragment_shader_factory(fragment_shader_code);
	fragment_shader_factory.set("<ambient>", material.ambient);
	fragment_shader_factory.set("<diffuse>", material.diffuse);
	fragment_shader_factory.set("<specular>", material.specular);
	fragment_shader_factory.set("<shininess>", material.shininess);
	fragment_shader_factory.set("<point_light_size>", m_point_lights.size());

	std::cout << fragment_shader_factory.code() << std::endl;
	renderer_t result (
		object::shader_t(object::shader_type_t::vertex, vertex_shader_code),
		object::shader_t(object::shader_type_t::fragment, fragment_shader_factory.code())
	);
	result.set_uniform_name(&uniform_description_t::model_matrix, "model_matrix");
	result.set_uniform_name(&uniform_description_t::view_projection, "view_projection");

	for(const auto& l : m_point_lights) {
		std::cout << l << std::endl;
	}

	result.set_uniform_name(&uniform_description_t::point_light_position, "point_light_position");
	result.set_uniform_array(&uniform_description_t::point_light_position, component_as_vector(m_point_lights, &point_light_t::position).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_ambient, "point_light_ambient");
	result.set_uniform_array(&uniform_description_t::point_light_ambient, component_as_vector(m_point_lights, &point_light_t::ambient).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_diffuse, "point_light_diffuse");
	result.set_uniform_array(&uniform_description_t::point_light_diffuse, component_as_vector(m_point_lights, &point_light_t::diffuse).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_specular, "point_light_specular");
	result.set_uniform_array(&uniform_description_t::point_light_specular, component_as_vector(m_point_lights, &point_light_t::specular).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_attenuation_constant, "point_light_attenuation_constant");
	result.set_uniform_array(&uniform_description_t::point_light_attenuation_constant, component_as_vector(m_point_lights, &point_light_t::attenuation_constant).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_attenuation_linear, "point_light_attenuation_linear");
	result.set_uniform_array(&uniform_description_t::point_light_attenuation_linear, component_as_vector(m_point_lights, &point_light_t::attenuation_linear).data(), m_point_lights.size());

	result.set_uniform_name(&uniform_description_t::point_light_attenuation_quadratic, "point_light_attenuation_quadratic");
	result.set_uniform_array(&uniform_description_t::point_light_attenuation_quadratic, component_as_vector(m_point_lights, &point_light_t::attenuation_quadratic).data(), m_point_lights.size());

	return result;
}

void blinn_phong_t::set_up(renderer_t& renderer, const material_t& material) const {

}

}
