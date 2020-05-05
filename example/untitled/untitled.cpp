#define GLM_FORCE_RADIANS
#include <glpp/core.hpp>
#include <glpp/system.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <fstream>
#include <thread>

#include <iostream>
#include <chrono>

struct scene_uniform_description_t{
	glm::mat4 mvp;
};

struct vertex_description_t {
	glm::vec3 position;
};

class model_t : public glpp::render::model_t<vertex_description_t> {
public:
	void add_quad(
		const glm::vec3& first,
		const glm::vec3& second,
		const glm::vec3& third,
		const glm::vec3& fourth
	) {
		emplace_back(vertex_description_t{first});
		emplace_back(vertex_description_t{second});
		emplace_back(vertex_description_t{third});

		emplace_back(vertex_description_t{first});
		emplace_back(vertex_description_t{third});
		emplace_back(vertex_description_t{fourth});
	}
};

struct camera_t {
	float fov;
	float aspect_ratio;
	float near = 1.0f;
	float far = 100.0f;
	glm::vec3 position;
	glm::quat orientation { glm::vec3(0.0) };

	glm::mat4 projection() {
		glm::mat4 result(1.0f);
		result = glm::perspective(fov, aspect_ratio, near, far);
		result = result*glm::mat4_cast(orientation);
		result = glm::translate(result, -position);
		return result;
	}
};

constexpr float g = 3;
struct plane_t {
	glm::vec3 position {0.0f, 0.0f, -3.0f};
	glm::vec3 speed {0, 0, -.01};
	glm::quat orientation { glm::vec3{0.0} };

	float throttle = 0;

	void update(std::chrono::duration<double> delta) {
		const glm::quat speed_orientation = glm::quatLookAt(glm::normalize(speed), orientation*glm::vec3(0,1,0));
		orientation = glm::slerp(orientation, speed_orientation, std::min(1.0f, std::pow(0.03f*glm::length(speed),2.0f)));
		const float d = delta.count();
// 		speed.y -= d*g;
		speed += orientation*glm::vec3(0,0,-throttle);
		position += d*speed;
		if(position.y < 0) {
			speed.y = 2*g;
		}
	}

	glm::mat4 mvp() const {
		return glm::translate(glm::mat4(1.0f), position)*glm::mat4_cast(orientation);
	}

	model_t model() const {
		return {{
			vertex_description_t{glm::vec3{   0,   0, 0}},
			vertex_description_t{glm::vec3{ 0.3,   0, 1}},
			vertex_description_t{glm::vec3{-0.3,   0, 1}},

			vertex_description_t{glm::vec3{   0,   0, 0}},
			vertex_description_t{glm::vec3{   0,   0, 1}},
			vertex_description_t{glm::vec3{   0, -0.3, 1}}
		}};
	}
};

int main(int, char*[]) {
	glpp::system::window_t window(800, 600, "input", glpp::system::vsync_t::off);

	using key_t = glpp::system::key_t;
	using action_t = glpp::system::action_t;

	camera_t camera { .fov = glm::radians(90.0f), .aspect_ratio = window.get_aspect_ratio(), .position = {0, 1, 0} };
	window.input_handler().set_keyboard_action(key_t::escape, action_t::press, [&](int) {
		window.close();
	});
	constexpr float step = 0.25;
	window.input_handler().set_keyboard_action(key_t::w, action_t::press, [&](int) {
		camera.position.z -= step;
	});
	window.input_handler().set_keyboard_action(key_t::s, action_t::press, [&](int) {
		camera.position.z += step;
	});
	window.input_handler().set_keyboard_action(key_t::a, action_t::press, [&](int) {
		camera.position.x -= step;
	});
	window.input_handler().set_keyboard_action(key_t::d, action_t::press, [&](int) {
		camera.position.x += step;
	});
	window.input_handler().set_keyboard_action(key_t::escape, action_t::press, [&](int) {
		window.close();
	});

	window.set_cursor_mode(glpp::system::cursor_mode_t::captured);

	window.input_handler().set_mouse_move_action([&](float x, float y) {
		camera.orientation =
			glm::angleAxis(
				glm::radians(0.1f*x),
				glm::vec3(
					0,1,0
				)
			);
		camera.orientation *=
			glm::angleAxis(
				glm::radians(0.1f*y),
				glm::inverse(camera.orientation)*glm::vec3(
					1,0,0
				)
			);
	});
	window.input_handler().set_keyboard_action(key_t::up, action_t::press, [&](int) {
		camera.orientation *= glm::quat(glm::vec3(glm::radians(5.0f),0,0));
	});
	window.input_handler().set_keyboard_action(key_t::down, action_t::press, [&](int) {
		camera.orientation *= glm::quat(glm::vec3(glm::radians(-5.0f),0,0));
	});
	window.input_handler().set_keyboard_action(key_t::left, action_t::press, [&](int) {
		camera.orientation *= glm::quat(glm::vec3(0,glm::radians(5.0f),0));
	});
	window.input_handler().set_keyboard_action(key_t::right, action_t::press, [&](int) {
		camera.orientation *= glm::quat(glm::vec3(0,glm::radians(-5.0f),0));
	});

	glpp::render::renderer_t<scene_uniform_description_t> renderer{
		glpp::object::shader_t(glpp::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::object::shader_t(glpp::object::shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};

	renderer.set_uniform_name( &scene_uniform_description_t::mvp, "mvp");

	glm::vec2 offset(0, 0);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	model_t model;
	for(float i = -100; i <= 100; ++i) {
		for(float j = -100; j <= 100; ++j) {
			model.add_quad(
				glm::vec3{-100.0f+i*100.0f, 0.0f, -100.0f+j*100.0f},
				glm::vec3{ 100.0f+i*100.0f, 0.0f, -100.0f+j*100.0f},
				glm::vec3{ 100.0f+i*100.0f, 0.0f,  100.0f+j*100.0f},
				glm::vec3{-100.0f+i*100.0f, 0.0f,  100.0f+j*100.0f}
			);
		}
	}
	model.add_quad(
		glm::vec3{-1.0f, -1.0f, -10.0f},
		glm::vec3{ 1.0f, -1.0f, -10.0f},
		glm::vec3{ 1.0f,  1.0f, -10.0f},
		glm::vec3{-1.0f,  1.0f, -10.0f}
	);

	glpp::render::view_t view(
		model,
		&vertex_description_t::position
	);

	plane_t plane;

	glpp::render::view_t plane_view(
		plane.model(),
		&vertex_description_t::position
	);

	glpp::call(glClearColor, 0.2,0.2,0.2,1.0);
	constexpr float fps = 60;
	auto& gamepad = window.input_handler().joysticks().front();
	using clock = std::chrono::high_resolution_clock;
	using duration_t = std::chrono::duration<double>;
	auto frame_begin = clock::now();
	window.enter_main_loop([&]() {
		std::this_thread::sleep_for(duration_t(1.0/fps));
		auto now = clock::now();
		plane.update(now-frame_begin);
		frame_begin = now;

		if(std::abs(gamepad.axes(0)) > .01 || std::abs(gamepad.axes(1)) > .01) {
			camera.orientation *=
			glm::angleAxis(
				glm::radians(gamepad.axes(0)*5),
						glm::vec3(
							0,1,0
						)
			);
			camera.orientation *=
			glm::angleAxis(
				glm::radians(gamepad.axes(1)*5),
						glm::inverse(camera.orientation)*glm::vec3(
							1,0,0
						)
			);
		}
		if(std::abs(gamepad.axes(3)) > .01 || std::abs(gamepad.axes(4)) > .01) {
			camera.position += (glm::inverse(camera.orientation)*glm::vec3(gamepad.axes(3), 0, gamepad.axes(4)))*glm::vec3(1,0,1);
		}

		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.set_uniform( &scene_uniform_description_t::mvp, camera.projection() );
		renderer.render(view);
		renderer.set_uniform( &scene_uniform_description_t::mvp, camera.projection()*plane.mvp() );
		renderer.render(plane_view);
	});

	return 0;
}
