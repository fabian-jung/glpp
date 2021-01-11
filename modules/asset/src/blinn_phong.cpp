#include "glpp/asset/shading/blinn_phong.hpp"


#warning
#include <iostream>

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

		ambient += vec3(.1);		
		diffuse += vec3(.1);
		specular += vec3(.1);
		
		FragColor = vec4(0,0,0,1);
		for(int i = 0; i < <point_light_size>; ++i) {
			float distance = distance(v_world_pos, point_light_position[i]);
			float LN = max(dot(v_norm,normalize(point_light_position[i]-v_world_pos)), 0.0);
			float attenuation = 
				1/(
// 					/*point_light_attenuation_constant[i]+*/1+
					/*point_light_attenuation_linear[i]**/distance+
					/*point_light_attenuation_quadratic[i]**/distance*distance
				);
			float Is = pow(LN, <shininess>);
			FragColor.xyz += point_light_ambient[i]*ambient;
			FragColor.xyz += point_light_diffuse[i]*diffuse*LN*attenuation;
			FragColor.xyz += point_light_specular[i]*specular*Is*attenuation;
		}
	};
)";

template <class T>
void find_and_replace(std::string& in, const std::string& matcher, const T& replace) {
	std::string::size_type pos;
	while((pos = in.find(matcher)) != in.npos) {
		in.erase(pos, matcher.length());
		in.insert(pos, std::to_string(replace));
	}
}

template <>
void find_and_replace(std::string& in, const std::string& matcher, const glm::vec3& replace) {
	std::string::size_type pos;
	while((pos = in.find(matcher)) != in.npos) {
		in.erase(pos, matcher.length());
		in.insert(pos, "vec3("+std::to_string(replace.x)+", "+std::to_string(replace.y)+", "+std::to_string(replace.z)+")");
	}
}

template <>
void find_and_replace(std::string& in, const std::string& matcher, const std::string& replace) {
	std::string::size_type pos;
	while((pos = in.find(matcher)) != in.npos) {
		in.erase(pos, matcher.length());
		in.insert(pos, replace);
	}
}

std::string blinn_phong_t::assemble_fragment_shader(const material_t& material) const {
	std::string code = fragment_shader_code;
	find_and_replace(code, "<ambient>", material.ambient);
	find_and_replace(code, "<diffuse>", material.diffuse);
	find_and_replace(code, "<specular>", material.specular);
	find_and_replace(code, "<point_light_size>", m_point_lights.size());
	find_and_replace(code, "<shininess>", material.shininess);
	
	std::cout << code << std::endl;
	return code;
}

template <class T, class U>
auto component_as_vector(std::vector<U> container, T U::* member) {
	std::vector<T> result;
	std::transform(container.begin(), container.end(), std::back_inserter(result), [member](const auto compound){ return compound.*member; });
	return result;
}

blinn_phong_t::renderer_t blinn_phong_t::renderer(const material_t& material) const {
	renderer_t result(
		object::shader_t(object::shader_type_t::vertex, vertex_shader_code),
		object::shader_t(object::shader_type_t::fragment, assemble_fragment_shader(material))
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

}
