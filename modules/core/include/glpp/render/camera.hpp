#pragma once

#include <glm/glm.hpp>

namespace glpp::render {



struct camera_t {
	glm::vec3 position = {0,0,0};
	glm::quat orientation = glm::quat(glm::vec3(0.0, 0.0, 0.0));
	float fov = 90;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	float aspect_ratio = 1.0f;

	glm::mat4 mvp(const glm::mat4 model_matrix = glm::mat4(1.0f)) const {
		const glm::mat4 v = glm::translate(model_matrix, -position);
		const auto r = glm::mat4_cast(orientation);
		const glm::mat4 p = glm::perspectiveFov(
			glm::radians(fov),
												aspect_ratio,
										1.0f,
										near_plane,
										far_plane
		);
		return p*r*v;
	}
};

}

