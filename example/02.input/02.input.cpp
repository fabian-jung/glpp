#include <glpp/system/window.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>

#include <fstream>

struct uniform_description_t{
	glm::vec3 color;
	glm::vec2 offset;
};

struct vertex_description_t {
	glm::vec2 position;
};

class model_t : public glpp::render::model_t<vertex_description_t> {
public:
	void add_quad(
		const glm::vec2& first,
		const glm::vec2& second,
		const glm::vec2& third,
		const glm::vec2& fourth
	) {
		m_verticies.reserve(m_verticies.size()+6);
		m_verticies.emplace_back(vertex_description_t{first});
		m_verticies.emplace_back(vertex_description_t{second});
		m_verticies.emplace_back(vertex_description_t{third});

		m_verticies.emplace_back(vertex_description_t{first});
		m_verticies.emplace_back(vertex_description_t{third});
		m_verticies.emplace_back(vertex_description_t{fourth});
	}
};

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


	glpp::render::renderer_t<uniform_description_t> renderer{
		glpp::object::shader_t(glpp::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::object::shader_t(glpp::object::shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};

	renderer.set_uniform_name( &uniform_description_t::color, "color");
	renderer.set_uniform_name( &uniform_description_t::offset, "offset");

	glm::vec2 offset(0, 0);
	glm::vec3 color(1.0f, 1.0f, 1.0f);
	renderer.set_uniform( &uniform_description_t::offset, offset);
	renderer.set_uniform( &uniform_description_t::color, color);

	model_t model;
	model.add_quad(
		glm::vec2{-0.25, -0.25},
		glm::vec2{ 0.25, -0.25},
		glm::vec2{ 0.25,  0.25},
		glm::vec2{-0.25,  0.25}
	);

	glpp::render::view_t<vertex_description_t> view(
		model,
		&vertex_description_t::position
	);

	window.input_handler().set_keyboard_action(key_t::r, action_t::press, [&](int) {
		color.x = 1-color.x;
		renderer.set_uniform( &uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::g, action_t::press, [&](int) {
		color.y = 1-color.y;
		renderer.set_uniform( &uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::b, action_t::press, [&](int) {
		color.z = 1-color.z;
		renderer.set_uniform( &uniform_description_t::color, color);
	});

	window.input_handler().set_keyboard_action(key_t::up, action_t::press, [&](int) {
		offset.y += 0.1f;
		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::down, action_t::press, [&](int) {
		offset.y -= 0.1f;
		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::left, action_t::press, [&](int) {
		offset.x -= 0.1f;
		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	window.input_handler().set_keyboard_action(key_t::right, action_t::press, [&](int) {
		offset.x += 0.1f;
		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	glm::vec2 mouse_drag_start;
	glm::vec2 offset_drag_start;

	bool dragging = false;
	window.input_handler().set_mouse_action(mouse_button_t::left, action_t::press, [&](double x, double y, int) {
		glm::vec2 mouse(
			(x/window.get_width())*2-1,
			(-y/window.get_height())*2+1
		);
		if(hit_check(offset, mouse)) {
			mouse_drag_start = mouse;
			offset_drag_start = offset;
			dragging = true;
		}
// 		renderer.set_uniform(&uniform_description_t::offset, offset);
	});

	window.input_handler().set_mouse_action(mouse_button_t::left, action_t::release, [&](double, double, int) {
		dragging = false;
	});

	window.input_handler().set_mouse_move_action([&](double x, double y){
		if(dragging) {
			glm::vec2 mouse(
				(x/window.get_width())*2-1,
				(-y/window.get_height())*2+1
			);
			offset = offset_drag_start + mouse - mouse_drag_start;
			renderer.set_uniform(&uniform_description_t::offset, offset);
		}
	});

	glpp::call(glClearColor, 0.2,0.2,0.2,1.0);
	window.enter_main_loop([&]() {
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}