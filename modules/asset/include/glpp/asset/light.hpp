#pragma once

#include <glm/glm.hpp>
#include <ostream>

namespace glpp::asset {

struct ambient_light_t {
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct directional_light_t {
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct point_light_t {
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
};

struct spot_light_t {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float inner_cone;
	float outer_cone;
	float attenuation_constant;
	float attenuation_linear;
	float attenuation_quadratic;
};

}

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::ambient_light_t& rhs);
std::ostream& operator<<(std::ostream& lhs, const glpp::asset::directional_light_t& rhs);
std::ostream& operator<<(std::ostream& lhs, const glpp::asset::point_light_t& rhs);
std::ostream& operator<<(std::ostream& lhs, const glpp::asset::spot_light_t& rhs);
