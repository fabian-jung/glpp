#include <catch2/catch.hpp>

#include <glm/gtx/string_cast.hpp>

#include <glpp/core/render/camera.hpp>
#include <iostream>

template <class T, auto N>
std::ostream& operator<<(std::ostream& lhs, const glm::vec<N, T>& rhs) {
	lhs << "(";
	for(auto i=0; i < rhs.length(); ++i) {
		lhs << rhs[i];
		if(i+1 < rhs.length()) {
			lhs << ", ";
		}
	}
	lhs << ")";
	return lhs;
}

TEST_CASE("core::camera_t default construction has sane default values", "[core][unit]") {
	const glpp::core::render::camera_t camera;

	REQUIRE(camera.fov != Approx(0.0f));
	REQUIRE(glm::length(camera.position) == 0.0f);
	REQUIRE(camera.orientation == glm::quat(glm::vec3(0.0, 0.0, 0.0)));
	REQUIRE(camera.fov > 30.0f);
	REQUIRE(camera.near_plane <    1.0f);
	REQUIRE(camera.near_plane >     0.05f);
	REQUIRE(camera.far_plane  < 1500.0f);
	REQUIRE(camera.far_plane  >    0.5f);
	REQUIRE(camera.aspect_ratio != Approx(0.0f).margin(0.1f));

}

TEST_CASE("Camera constructed with parameters", "[core][unit]") {
	const glm::vec3 position { 1.0f, 1.0f, 1.0f};
	const glm::quat orientation {glm::vec3(45.0f, 37.5f, 90.0f)};
	const float fov = 45.0f;
	const float near_plane = 1.0f;
	const float far_plane = 10.0f;
	const float aspect_ratio = 2.0f;

	const glpp::core::render::camera_t camera(
		position,
		orientation,
		fov,
		near_plane,
		far_plane,
		aspect_ratio
	);

	REQUIRE(camera.position == position);
	REQUIRE(camera.orientation == orientation);
	REQUIRE(camera.fov == fov);
	REQUIRE(camera.near_plane == near_plane);
	REQUIRE(camera.far_plane  == far_plane);
	REQUIRE(camera.aspect_ratio == aspect_ratio);
}

void compare_cameras(const glpp::core::render::camera_t& lhs, const glpp::core::render::camera_t& rhs) {
	REQUIRE(lhs.position == rhs.position);
	const glm::vec3 up {0,1,0};
	const glm::vec3 forward {0,0,-1};
	const glm::vec3 right {1,0,0};

	REQUIRE(glm::length(up*lhs.orientation-up*rhs.orientation) < 0.01);
	REQUIRE(glm::length(forward*lhs.orientation-forward*rhs.orientation) < 0.01);
	REQUIRE(glm::length(right*lhs.orientation-right*rhs.orientation) < 0.01);
}

TEST_CASE("Camera constructed with look_at and up vector", "[core][unit]") {
	using glpp::core::render::camera_t;

	const glm::vec3 position {0.0f, 0.0f,  0.0f};
	const glm::vec3  look_at {0.0f, 0.0f, -1.0f};
	const glm::vec3       up {0.0f, 1.0f,  0.0f};
	constexpr auto angle = glm::radians(90.0f);

	const std::array<glm::vec3, 3> axes {
		glm::vec3{1.0f, 0.0f, 0.0f},
		glm::vec3{0.0f, 1.0f, 0.0f},
		glm::vec3{0.0f, 0.0f, 1.0f}
	};

	for(const auto& offset : axes) {
		DYNAMIC_SECTION("Translate camera by "+glm::to_string(offset)) {
			const camera_t look_at_camera(
				position+offset,
				look_at+offset,
				up+offset
			);

			const camera_t reference_camera(
				position+offset,
				glm::quat(glm::vec3(0.0f, 0.0f, 0.0f))
			);

			compare_cameras(look_at_camera, reference_camera);
		}
	}

	for(const auto& axis : axes) {
		DYNAMIC_SECTION("Rotate camera around "+glm::to_string(axis)) {
			const camera_t look_at_camera(
				position,
				glm::rotate(look_at, angle, axis),
				glm::rotate(     up, angle, axis)
			);

			const camera_t reference_camera(
				position,
				glm::angleAxis(angle, axis)
			);

			compare_cameras(look_at_camera, reference_camera);
		}
	}

	for(float x = -1.0f; x < 1.0f; x+=1.0f) {
	for(float y = -1.0f; y < 1.0f; y+=1.0f) {
	for(float z = -1.0f; z < 1.0f; z+=1.0f) {
	for(const auto& axis : axes) {
		const glm::vec3 offset { x, y, z };
		DYNAMIC_SECTION("Translate camera by "+glm::to_string(offset)+" and rotate around "+glm::to_string(axis)) {
			const camera_t look_at_camera(
				offset+position,
				offset+glm::rotate(look_at, angle, axis),
				offset+glm::rotate(     up, angle, axis)
			);

			const camera_t reference_camera(
				offset+position,
				glm::angleAxis(angle, axis)
			);

			compare_cameras(look_at_camera, reference_camera);
		}
	}
	}
	}
	}

}

TEST_CASE("core::camera_t::mvp() sanity checks of the projection matrix", "[core][unit]") {
	const glpp::core::render::camera_t camera;
	const auto mvp = camera.mvp();

	SECTION("Centerpoint on near_plane projected to back") {
		const auto world_space_point = glm::vec4(0.0f, 0.0f, -camera.far_plane, 1.0f);
		const auto camera_space_point = mvp*world_space_point;
		const auto clip_space_point = camera_space_point*(1.0f/camera_space_point.w);

		REQUIRE(clip_space_point.x ==  Approx(0.0f).margin(0.01f));
		REQUIRE(clip_space_point.y ==  Approx(0.0f).margin(0.01f));
		REQUIRE(clip_space_point.z ==  Approx(1.0f).margin(0.01f));
		REQUIRE(clip_space_point.w ==  Approx(1.0f).margin(0.01f));
	}

	SECTION("Centerpoint on near_plane projected to back") {
		const auto world_space_point = glm::vec4(0.0f, 0.0f, -camera.near_plane, 1.0f);
		const auto camera_space_point = mvp*world_space_point;
		const auto clip_space_point = camera_space_point*(1.0f/camera_space_point.w);

		REQUIRE(clip_space_point.x ==  Approx(0.0f).margin(0.01f));
		REQUIRE(clip_space_point.y ==  Approx(0.0f).margin(0.01f));
		REQUIRE(clip_space_point.z == Approx(-1.0f).margin(0.01f));
		REQUIRE(clip_space_point.w ==  Approx(1.0f).margin(0.01f));
	}
}

TEST_CASE("Construct camera with look_at vector. look_at should be projected to the middle:", "[core][unit]") {
	const auto offsets = [](){
		std::vector<glm::vec3> container;
		for(float x = -1.0f; x <= 1.0f; ++x) {
			for(float y = -1.0f; y <= 1.0f; ++y) {
				for(float z = -1.0f; z <= 1.0f; ++z) {
					container.emplace_back(x, y, z);
				}
			}
		}
		return container;
	}();

	for(const auto& pos_off : std::vector<glm::vec3>{glm::vec3{0, 0, 0}}) {
		for(const auto& la_off : offsets) {
			if(glm::length(la_off) == 0.0f) continue;
			const glm::vec3 position = pos_off;
			const glm::vec3 look_at = position+la_off;
			const glm::vec3 up = position + glm::vec3{ la_off.x, -la_off.z, la_off.y };
			DYNAMIC_SECTION(
				"look from ( " << position.x << " " << position.y << " " << position.z  << " ) to ( "
				<< look_at.x << " " << look_at.y << " " << look_at.z << " ), up = ( "
				<< up.x << " " << up.y << " " << up.z << " )"
			) {

				const glpp::core::render::camera_t camera(
					position,
					look_at,
					up
				);

				const auto mvp = camera.mvp();

				const auto projected = mvp*glm::vec4(look_at, 1.0f);
				REQUIRE(projected.x ==  Approx(0.0f).margin(0.01f));
				REQUIRE(projected.y ==  Approx(0.0f).margin(0.01f));
			}
		}
	}
}