#include <glpp/system/window.hpp>
#include <glpp/core/render/renderer.hpp>
#include <glpp/core/render/model.hpp>
#include <glpp/core/render/view.hpp>

#include <fstream>

struct scene_uniform_description_t{
	glm::vec3 color;
	glm::vec2 offset;
};

struct vertex_description_t {
	glm::vec2 position;
};

class model_t : public glpp::core::render::model_t<vertex_description_t> {
public:
	void add_quad(
		const glm::vec2& first,
		const glm::vec2& second,
		const glm::vec2& third,
		const glm::vec2& fourth
	) {
		emplace_back(vertex_description_t{first});
		emplace_back(vertex_description_t{second});
		emplace_back(vertex_description_t{third});

		emplace_back(vertex_description_t{first});
		emplace_back(vertex_description_t{third});
		emplace_back(vertex_description_t{fourth});
	}
};

namespace glpp::core::render {
template <>
struct model_traits<::model_t> : public model_traits<glpp::core::render::model_t<vertex_description_t>> {};
}

bool hit_check(const glm::vec2& offset, const glm::vec2& mouse) {
	const glm::vec2 pos = mouse - offset + glm::vec2(0.25, 0.25);
	return pos.x > 0 && pos.x < 0.5 && pos.y > 0 && pos.y < 0.5;
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
	glpp::system::window_t window(800, 600, "input");
	window.set_input_mode(glpp::system::input_mode_t::wait);

	using key_t = glpp::system::key_t;
	using action_t = glpp::system::action_t;
	using mouse_button_t = glpp::system::mouse_button_t;

	window.input_handler().set_keyboard_action(key_t::escape, action_t::press, [&](int) {
		window.close();
	});


	glpp::core::render::renderer_t<scene_uniform_description_t> renderer{
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};

	renderer.set_uniform_name( &scene_uniform_description_t::color, "color");
	renderer.set_uniform_name( &scene_uniform_description_t::offset, "offset");

	glm::vec2 offset(0, 0);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	renderer.set_uniform( &scene_uniform_description_t::offset, offset);
	renderer.set_uniform( &scene_uniform_description_t::color, color);

	model_t model;
	model.add_quad(
		glm::vec2{-0.25, -0.25},
		glm::vec2{ 0.25, -0.25},
		glm::vec2{ 0.25,  0.25},
		glm::vec2{-0.25,  0.25}
	);

	glpp::core::render::view_t view(
		model
	);

	window.input_handler().set_keyboard_action(key_t::r, action_t::press, [&](int) {
		color.x = 1-color.x;
		renderer.set_uniform( &scene_uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::g, action_t::press, [&](int) {
		color.y = 1-color.y;
		renderer.set_uniform( &scene_uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::b, action_t::press, [&](int) {
		color.z = 1-color.z;
		renderer.set_uniform( &scene_uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::up, action_t::press, [&](int) {
		offset.y += 0.1f;
		renderer.set_uniform(&scene_uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::down, action_t::press, [&](int) {
		offset.y -= 0.1f;
		renderer.set_uniform(&scene_uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::left, action_t::press, [&](int) {
		offset.x -= 0.1f;
		renderer.set_uniform(&scene_uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::right, action_t::press, [&](int) {
		offset.x += 0.1f;
		renderer.set_uniform(&scene_uniform_description_t::offset, offset);
	});

	glm::vec2 mouse_drag_start;
	glm::vec2 offset_drag_start;

	bool dragging = false;
	window.input_handler().set_mouse_action(mouse_button_t::left, action_t::press, [&](glm::vec2 mouse, int) {
		if(hit_check(offset, mouse)) {
			mouse_drag_start = mouse;
			offset_drag_start = offset;
			dragging = true;
		}
// 		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	window.input_handler().set_mouse_action(mouse_button_t::left, action_t::release, [&](glm::vec2, int) {
		dragging = false;
	});

	window.input_handler().set_mouse_move_action([&](glm::vec2 dst, glm::vec2){
		if(dragging) {
			offset = offset_drag_start + dst - mouse_drag_start;
			renderer.set_uniform(&scene_uniform_description_t::offset, offset);
		}
	});

	glClearColor(0.2,0.2,0.2,1.0);
	window.enter_main_loop([&]() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
