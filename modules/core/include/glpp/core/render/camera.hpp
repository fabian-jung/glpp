#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace glpp::core::render {

struct camera_t {

	glm::vec3 position = {0,0,0};
	glm::quat orientation = glm::quat(glm::vec3(0.0, 0.0, 0.0));
	float fov = 90;
	float near_plane = 0.1f;
	float far_plane = 100.0f;
	float aspect_ratio = 1.0f;

	camera_t() = default;

	camera_t(
		glm::vec3 position,
		glm::quat orientation = glm::quat(glm::vec3(0.0, 0.0, 0.0)),
		float fov = 90,
		float near_plane = 0.1f,
		float far_plane = 100.0f,
		float aspect_ratio = 1.0f
	);

	camera_t(
		glm::vec3 position,
		glm::vec3 lookAt,
		glm::vec3 up,
		float fov = 90,
		float near_plane = 0.1f,
		float far_plane = 100.0f,
		float aspect_ratio = 1.0f
	);

	glm::mat4 mvp(const glm::mat4& model_matrix = glm::mat4(1.0f)) const;

};

}
