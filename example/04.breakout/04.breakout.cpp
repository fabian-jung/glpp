#include <glpp/system/window.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>
#include <glpp/object/texture.hpp>
#include "ball_motion_handler.hpp"
#include <fstream>

struct second_stage_uniform_description_t {
};

struct position_t {
	glm::vec2 position;
};

struct tile_uniform_t {
	glm::ivec2 bounds;
};

struct vertex_description_t {
	glm::vec2 position;
	glm::vec2 tex;
};

class quad_model_t : public glpp::render::model_t<vertex_description_t> {
public:
	quad_model_t(std::initializer_list<std::pair<glm::vec2, glm::vec2>> list) {
		for(const auto& p : list) {
			add_quad(p.first, p.second);
		}
	}

	void add_quad(
		const vertex_description_t& first,
		const vertex_description_t& second,
		const vertex_description_t& third,
		const vertex_description_t& fourth
	) {
		emplace_back(first);
		emplace_back(second);
		emplace_back(third);

		emplace_back(first);
		emplace_back(third);
		emplace_back(fourth);
	}

	void add_quad(const glm::vec2& position, const glm::vec2& dimension) {
		const auto left_down = position - 0.5f*dimension;
		add_quad(
			vertex_description_t{left_down, {0.0, 0.0}},
			vertex_description_t{left_down+glm::vec2(dimension.x, 0.0), {1.0, 0.0}},
			vertex_description_t{left_down+dimension, {1.0, 1.0}},
			vertex_description_t{left_down+glm::vec2(0.0, dimension.y), {0.0, 1.0}}
		);
	}
};

namespace glpp::render {
	template <>
	struct model_traits<quad_model_t> : public model_traits<glpp::render::model_t<vertex_description_t>> {};
}

using namespace glpp::system;
using namespace glpp::object;
using namespace glpp::render;

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
	window_t window(800, 600, "breakout");
// 	window.set_cursor_mode(glpp::system::cursor_mode_t::hidden);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.2,0.2,0.2,1.0);
	window.input_handler().set_keyboard_action(glpp::system::key_t::escape, glpp::system::action_t::press, [&](int){
		window.close();
	});

	static ball_motion_handler_t mh;

	window.input_handler().set_mouse_move_action([&](glm::vec2 mouse, glm::vec2){
		mh.set_slider(mouse.x);
// 		ball_renderer.set_uniform(&position_t::position, mouse);
	});

	window.input_handler().set_keyboard_action(glpp::system::key_t::space, glpp::system::action_t::release, [&](int){
		mh.start();
	});

	struct {
		renderer_t<tile_uniform_t> renderer{
			shader_t(shader_type_t::vertex, std::ifstream("instance_vertex.glsl")),
			shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
		};
		texture_t texture{image_t<glm::vec3>("Tile.png")};
		texture_slot_t tex_slot = texture.bind_to_texture_slot();
		texture_slot_t level_tex_slot = mh.level_texture().bind_to_texture_slot();
		view_t<quad_model_t> view{
			quad_model_t{{glm::vec2(1.0/mh.level().width(), 1.0/mh.level().height()), glm::vec2(2.0/mh.level().width(), 2.0/mh.level().height())}},
			&vertex_description_t::position,
			&vertex_description_t::tex
		};
	} tile;
	tile.renderer.set_uniform_name(&tile_uniform_t::bounds, "bounds");
	tile.renderer.set_texture("texture_unit", tile.tex_slot);
	tile.renderer.set_uniform(&tile_uniform_t::bounds, glm::ivec2(mh.level().width(), mh.level().height()));
	tile.renderer.set_texture("level", tile.level_tex_slot);

	struct {
		renderer_t<position_t> renderer{
			shader_t(shader_type_t::vertex, std::ifstream("position_vertex.glsl")),
			shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
		};
		texture_t texture{image_t<glm::vec3>("Slider.png")};
		texture_slot_t tex_slot = texture.bind_to_texture_slot();
		view_t<quad_model_t> view{
			quad_model_t{{glm::vec2(0, 0), glm::vec2(mh.slider_width(), 0.05)}},
			&vertex_description_t::position,
			&vertex_description_t::tex
		};
	} slider;
	slider.renderer.set_uniform_name(&position_t::position, "position");
	slider.renderer.set_uniform(&position_t::position, glm::vec2(0, -0.95));
	slider.renderer.set_texture("texture_unit", slider.tex_slot);

	struct {
		renderer_t<position_t> renderer{
			shader_t(shader_type_t::vertex, std::ifstream("position_vertex.glsl")),
			shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
		};
		texture_t texture{image_t<glm::vec3>("Ball.png")};
		texture_slot_t tex_slot = texture.bind_to_texture_slot();
		view_t<quad_model_t> view{
			quad_model_t{{glm::vec2(0.0, 0.0), glm::vec2(mh.size(), mh.size())}},
			&vertex_description_t::position,
			&vertex_description_t::tex
		};
	} ball;
	ball.renderer.set_uniform_name(&position_t::position, "position");
	ball.renderer.set_texture("texture_unit", ball.tex_slot);


	window.enter_main_loop([&]() {
		mh.update();
		ball.renderer.set_uniform(&position_t::position, mh.position());
		slider.renderer.set_uniform(&position_t::position, glm::vec2(mh.get_slider(), -0.95));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		tile.renderer.render_instanced(tile.view, mh.level().width()*mh.level().height());
		slider.renderer.render(slider.view);
		ball.renderer.render(ball.view);

	});

	return 0;
}
