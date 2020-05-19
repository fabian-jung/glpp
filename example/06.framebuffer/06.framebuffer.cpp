#include <glpp/system.hpp>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include "cube_model.hpp"
#include "quad_model.hpp"

#include <iostream>

using namespace glpp::system;
using namespace glpp::object;
using namespace glpp::render;

struct scene_uniform_description_t {
	glm::mat4 mvp;
};

struct postprocessing_uniform_description_t {
	glm::vec2     resolution;
	bool          direction;
};

struct second_stage_uniform_description_t {
	float depth_in_focus;
};

int main(
	__attribute__((unused)) int,
	__attribute__((unused)) char **
) {
	window_t window(800, 600, "05.framebuffer", glpp::system::vsync_t::off);
	window.input_handler().set_keyboard_action(
		glpp::system::key_t::escape,
		action_t::press,
		[&](int){
			window.close();
		}
	);

	renderer_t<scene_uniform_description_t> scene_renderer {
		shader_t(shader_type_t::vertex, std::ifstream("vertex.glsl")),
		shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};
	scene_renderer.set_uniform_name(&scene_uniform_description_t::mvp, "mvp");

	renderer_t<postprocessing_uniform_description_t> first_stage_postprocessing {
		shader_t(shader_type_t::vertex, std::ifstream("pp_vertex.glsl")),
		shader_t(shader_type_t::fragment, std::ifstream("box_filter_fragment.glsl"))
	};
	first_stage_postprocessing.set_uniform_name(&postprocessing_uniform_description_t::resolution, "resolution");
	first_stage_postprocessing.set_uniform_name(&postprocessing_uniform_description_t::direction, "direction");

	renderer_t<second_stage_uniform_description_t> second_stage_postprocessing {
		shader_t(shader_type_t::vertex, std::ifstream("pp_vertex.glsl")),
		shader_t(shader_type_t::fragment, std::ifstream("dof_fragment.glsl"))
	};
	second_stage_postprocessing.set_uniform_name(&second_stage_uniform_description_t::depth_in_focus, "depth_in_focus");

	view_t<cube_vertex_description_t> scene(
		cube_model_t{
			{{0,0,0},  {2,2,2}},
			{{0,-1,0},  {20,0,20}}
		},
		&cube_vertex_description_t::position,
		&cube_vertex_description_t::norm
	);

	view_t<quad_vertex_description_t> screen_quad{
		quad_model_t{
			{{0,0}, {2, 2}}
		},
		&quad_vertex_description_t::position,
		&quad_vertex_description_t::tex
	};

	glpp::call(glClearColor, 0.2,0.2,0.2,1.0);
	constexpr float fov = 45;
	constexpr float z_near = 0.1;
	constexpr float z_far = 10.0;

	glm::mat4 m(1.0f);
	m = glm::rotate(m, glm::radians(45.0f), glm::vec3(1, 0, 0));

	const glm::mat4 v = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-5));
	const glm::mat4 p = glm::perspectiveFov(
		glm::radians(fov),
		static_cast<float>(window.get_width()),
		static_cast<float>(window.get_height()),
		z_near,
		z_far
	);

	texture_t depth_target(
		window.get_width(),
		window.get_height(),
		image_format_t::d_32f
	);

	struct pass_t {
		texture_t color_target;
		framebuffer_t framebuffer;
	};

	std::array<pass_t, 3> passes {
		pass_t{
			texture_t{window.get_width(), window.get_height(),image_format_t::rgb_8, clamp_mode_t::clamp_to_edge},
			framebuffer_t{{depth_target, attachment_t::depth}}
		},
		pass_t{
			texture_t{window.get_width(), window.get_height(),image_format_t::rgb_8, clamp_mode_t::clamp_to_edge},
			framebuffer_t{window.get_width(), window.get_height()}
		},
		pass_t{
			texture_t{window.get_width(), window.get_height(),image_format_t::rgb_8, clamp_mode_t::clamp_to_edge},
			framebuffer_t{window.get_width(), window.get_height()}
		}
	};

	auto& first_pass = passes[0];
	auto& second_pass = passes[1];
	auto& third_pass = passes[2];

	for(auto& p : passes) {
		p.framebuffer.attach(p.color_target, attachment_t::color);
	}


	auto first_pass_slot = first_pass.color_target.bind_to_texture_slot();
	auto second_pass_slot = second_pass.color_target.bind_to_texture_slot();
	auto third_pass_slot = second_pass.color_target.bind_to_texture_slot();
	auto depth_slot = depth_target.bind_to_texture_slot();

	second_stage_postprocessing.set_texture("original", first_pass_slot);
	second_stage_postprocessing.set_texture("blurred", second_pass_slot);
	second_stage_postprocessing.set_texture("depth", depth_slot);

	constexpr bool x_direction = true;
	constexpr bool y_direction = false;

	glm::vec2 mouse;
	window.input_handler().set_mouse_move_action([&](glm::vec2 dst, glm::vec2){
		mouse = dst;
	});

	window.enter_main_loop([&]() {
		m = glm::rotate(m, glm::radians(0.01f), glm::vec3(0,1,0));
		const auto mvp = p*v*m;
		scene_renderer.set_uniform(&scene_uniform_description_t::mvp, mvp);

		// Render first pass
		// 3D Scene with color and depth buffer
		first_pass.framebuffer.bind(framebuffer_target_t::read_and_write);
		glpp::call(glViewport, 0, 0, window.get_width(), window.get_height());
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scene_renderer.render(scene);


		// The blur filter is seperable and split into two stages
		// Render second pass -- box filter in x-direction
		first_stage_postprocessing.set_uniform(&postprocessing_uniform_description_t::resolution, glm::vec2(window.get_width(), window.get_height()));
		first_stage_postprocessing.set_uniform(&postprocessing_uniform_description_t::direction, x_direction);
		second_pass.framebuffer.bind(framebuffer_target_t::write);
		first_stage_postprocessing.set_texture("slot", first_pass_slot);
		glpp::call(glViewport, 0, 0, window.get_width(), window.get_height());
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		first_stage_postprocessing.render(screen_quad);

		// Render third pass -- box filter in y-direction
		first_stage_postprocessing.set_uniform(&postprocessing_uniform_description_t::direction, y_direction);
		third_pass.framebuffer.bind(framebuffer_target_t::write);
		first_stage_postprocessing.set_texture("slot", second_pass_slot);
		glpp::call(glViewport, 0, 0, window.get_width(), window.get_height());
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		first_stage_postprocessing.render(screen_quad);

		// The final pass does the dof calculation
		// it interpolates between the blurred image and the original scene depending on the difference in the depth-buffer
		// to the depth under the mouse
		framebuffer_t::bind_default_framebuffer(framebuffer_target_t::write);
		float f;
		glpp::call(glReadPixels, mouse.x, window.get_height()-mouse.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &f);
		second_stage_postprocessing.set_uniform(&second_stage_uniform_description_t::depth_in_focus, f);

		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		second_stage_postprocessing.render(screen_quad);
	});

	return 0;
}
