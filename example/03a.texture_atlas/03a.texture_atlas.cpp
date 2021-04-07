#include <glpp/system/window.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>
#include <glpp/object/texture.hpp>
#include <glpp/object/texture_atlas.hpp>
#include <glpp/object/shader_factory.hpp>

#include <fstream>
#include <string_view>
#include <unordered_map>

#include <iostream>

struct scene_uniform_description_t{
};

struct vertex_description_t {
	glm::vec2 position;
	glm::vec2 tex;
};

class model_t : public glpp::render::model_t<vertex_description_t> {
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

namespace glpp::render {
	template <>
	struct model_traits<::model_t> : public model_traits<glpp::render::model_t<vertex_description_t>> {};
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {

	glpp::system::window_t window(800, 600, "texture", glpp::system::vsync_t::off);
	window.set_input_mode(glpp::system::input_mode_t::wait);


	glpp::object::multi_atlas_t texture_atlas;
	const auto zero = texture_atlas.alloc(glpp::object::image_t<glm::vec3>(1, 1, { glm::vec3(1.0,0,0)}));
	const auto first = texture_atlas.alloc(glpp::object::image_t<glm::vec3>("one.png"));
	const auto second = texture_atlas.alloc(glpp::object::image_t<glm::vec3>("two.png"));


	glpp::object::shader_factory_t fragment_shader_factory(std::ifstream("fragment.glsl"));
	fragment_shader_factory.set("<texture_atlas>", texture_atlas.declaration());
	fragment_shader_factory.set("<fetch>", texture_atlas.fetch(first, "tex"));
	std::cout << fragment_shader_factory.code() << std::endl;
	glpp::render::renderer_t<scene_uniform_description_t> renderer {
		glpp::object::shader_t(glpp::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::object::shader_t(glpp::object::shader_type_t::fragment, fragment_shader_factory.code())
	};

	model_t model;
	model.add_quad(
		vertex_description_t{{-0.75, -0.75}, {0, 0}},
		vertex_description_t{{ 0.75, -0.75}, {1, 0}},
		vertex_description_t{{ 0.75,  0.75}, {1, 1}},
		vertex_description_t{{-0.75,  0.75}, {0, 1}}
	);

	glpp::object::texture_t texture_one(
		glpp::object::image_t<glm::vec3>(
			"smiley.png"
		),             
		glpp::object::image_format_t::prefered,
		glpp::object::clamp_mode_t::clamp_to_edge,
		glpp::object::filter_mode_t::linear
	);
	const auto tu = texture_atlas.bind_to_texture_slot();
	//renderer.set_texture_atlas_slot(tu);
	
	// const auto texture_unit_one = texture_one.bind_to_texture_slot();
	const auto ta_slot = texture_atlas.bind_to_texture_slot();
	//renderer.set_texture("texture_one" , texture_unit_one);
	renderer.set_texture_array(texture_atlas.texture_id().c_str(), ta_slot.begin(), ta_slot.end());

	glpp::render::view_t view(
		model,
		&vertex_description_t::position,
		&vertex_description_t::tex
	);
	
	glClearColor(0.2,0.2,0.2,1.0);
	window.enter_main_loop([&]() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
