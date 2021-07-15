#include <glpp/core/render/camera.hpp>
#include <glm/gtx/vector_angle.hpp>

glpp::core::render::camera_t::camera_t(
	glm::vec3 position,
	glm::quat orientation,
	float fov,
	float near_plane,
	float far_plane,
	float aspect_ratio
) noexcept :
	position(position),
	orientation(orientation),
	fov(fov),
	near_plane(near_plane),
	far_plane(far_plane),
	aspect_ratio(aspect_ratio)
{}


glm::quat orientationFromLookAt(const glm::vec3& look_at, const glm::vec3& up) {
	glm::vec3 forward(0.0f, 0.0f, -1.0f);
	const auto forward_rotation_axis = glm::normalize(glm::cross(look_at, forward));
	const float forward_angle = glm::orientedAngle(forward, look_at, forward_rotation_axis);
	const auto forward_rotation =
		(forward_rotation_axis==forward_rotation_axis) ?
			glm::angleAxis(forward_angle, forward_rotation_axis) :
			glm::quat(glm::vec3(0.0f));

	const auto up_ref = forward_rotation*glm::vec3(0.0f, 1.0f, 0.0f);
	const auto up_rotation_axis = forward_rotation*glm::vec3(0.0f, 0.0f, 1.0f);

	const float up_angle = glm::orientedAngle(up_ref, up, up_rotation_axis);
	const auto up_rotation = glm::angleAxis(up_angle, up_rotation_axis);

	return up_rotation*forward_rotation;
}

glpp::core::render::camera_t::camera_t(
	glm::vec3 position,
	glm::vec3 lookAt,
	glm::vec3 up,
	float fov,
	float near_plane,
	float far_plane,
	float aspect_ratio
) noexcept  :
	position(position),
	orientation(orientationFromLookAt(glm::normalize(lookAt-position), glm::normalize(up-position))),
	fov(fov),
	near_plane(near_plane),
	far_plane(far_plane),
	aspect_ratio(aspect_ratio)
{}

glm::mat4 glpp::core::render::camera_t::mvp(const glm::mat4& model_matrix) const {
	const glm::mat4 translation = glm::translate(glm::mat4(1.0f), -position);
	const auto rotation = glm::mat4_cast(glm::inverse(orientation));
	const glm::mat4 projection = glm::perspectiveFov(
		glm::radians(fov),
		aspect_ratio,
		1.0f,
		near_plane,
		far_plane
	);
	return projection*rotation*translation*model_matrix;
}