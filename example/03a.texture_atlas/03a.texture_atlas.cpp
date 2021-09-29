#include <glpp/system/window.hpp>
#include <glpp/core.hpp>

#include <fstream>
#include <string_view>
#include <unordered_map>
#include <chrono>
#include <iostream>

struct scene_uniform_description_t{
	float tex_id;
};

struct vertex_description_t {
	glm::vec2 position;
	glm::vec2 tex;
};

class model_t : public glpp::core::render::model_t<vertex_description_t> {
public:
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
};

namespace glpp::core::render {
	template <>
	struct model_traits<::model_t> : public model_traits<model_t<vertex_description_t>> {};
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {

	glpp::system::window_t window(800, 600, "texture", glpp::system::vsync_t::on);

	// glpp::core::object::multi_atlas_t texture_atlas;
	glpp::core::object::grid_atlas_t texture_atlas { 2, 2, 2048, 2048 };

	texture_atlas.insert(glpp::core::object::image_t<glm::vec3>("one.png"));
	texture_atlas.insert(glpp::core::object::image_t<glm::vec3>("two.png"));
	texture_atlas.insert(glpp::core::object::image_t<glm::vec3>("three.png"));
	texture_atlas.insert(glpp::core::object::image_t<glm::vec3>("four.png"));


	glpp::core::object::shader_factory_t fragment_shader_factory(std::ifstream("fragment.glsl"));
	fragment_shader_factory.set("<texture_atlas>", texture_atlas.declaration("textures"));
	fragment_shader_factory.set("<fetch1>", texture_atlas.dynamic_fetch("textures", "first_index", "tex"));
	fragment_shader_factory.set("<fetch2>", texture_atlas.dynamic_fetch("textures", "second_index", "tex"));
	std::cout << fragment_shader_factory.code() << std::endl;
	glpp::core::render::renderer_t<scene_uniform_description_t> renderer {
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::fragment, fragment_shader_factory.code())
	};
	renderer.set_uniform_name(&scene_uniform_description_t::tex_id, "tex_id");

	model_t model;
	model.add_quad(
		vertex_description_t{{-1.0, -1.0}, {0, 0}},
		vertex_description_t{{ 1.0, -1.0}, {1, 0}},
		vertex_description_t{{ 1.0,  1.0}, {1, 1}},
		vertex_description_t{{-1.0,  1.0}, {0, 1}}
	);

	glpp::core::object::texture_t texture_one(
		glpp::core::object::image_t<glm::vec3>(
			"smiley.png"
		),             
		glpp::core::object::image_format_t::preferred,
		glpp::core::object::clamp_mode_t::clamp_to_edge,
		glpp::core::object::filter_mode_t::linear
	);
	const auto tu = texture_atlas.bind_to_texture_slot();
	
	// const auto texture_unit_one = texture_one.bind_to_texture_slot();
	const auto ta_slot = texture_atlas.bind_to_texture_slot();
	renderer.set_texture_atlas("textures", ta_slot);

	glpp::core::render::view_t view(
		model
	);
	
	glClearColor(0.2,0.2,0.2,1.0);

	using clock = std::chrono::high_resolution_clock;
	const auto begin = clock::now();

	window.enter_main_loop([&]() {
		const auto now = clock::now();
		const std::chrono::duration<float> duration = now-begin;
		const auto time = duration.count()*.2f;
		renderer.set_uniform(&scene_uniform_description_t::tex_id, time);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
