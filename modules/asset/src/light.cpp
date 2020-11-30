#include <glpp/asset/light.hpp>
#include <glm/gtx/string_cast.hpp>

template <class T, auto N>
std::ostream& operator<<(std::ostream& lhs, const glm::vec<N, T>& rhs) {
	lhs << glm::to_string(rhs);
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::ambient_light_t& rhs) {
	lhs << "ambient_light_t( ambient=" << rhs.ambient << ", diffuse=" << rhs.diffuse << ", specular="<< rhs.specular <<" )";
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::directional_light_t& rhs) {
	lhs << "directional_light_t( direction=" << rhs.direction << ", ambient=" << rhs.ambient << ", diffuse=" << rhs.diffuse << ", specular="<< rhs.specular <<" )";
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::point_light_t& rhs) {
	lhs << "point_light_t( position=" << rhs.position << ", ambient=" << rhs.ambient << ", diffuse=" << rhs.diffuse << ", specular="<< rhs.specular << ", lafc="<< rhs.attenuation_constant << ", lafl="<< rhs.attenuation_linear << ", lafq="<< rhs.attenuation_quadratic << " )";
	return lhs;
}

std::ostream& operator<<(std::ostream& lhs, const glpp::asset::spot_light_t& rhs) {
	lhs << "point_light_t( position=" << rhs.position << ", direction=" << rhs.direction << ", ambient=" << rhs.ambient << ", diffuse=" << rhs.diffuse << ", specular="<< rhs.specular << ", inner_cone_angle="<< rhs.inner_cone << ", outer_cone_angle="<< rhs.outer_cone << ", lafc="<< rhs.attenuation_constant << ", lafl="<< rhs.attenuation_linear << ", lafq="<< rhs.attenuation_quadratic << " )";
	return lhs;
}
