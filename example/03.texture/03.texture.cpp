#include <glpp/system/window.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>
#include <glpp/object/texture.hpp>

#include <fstream>

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

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
	glpp::system::window_t window(800, 600, "texture");
	window.set_input_mode(glpp::system::input_mode_t::wait);

	glpp::render::renderer_t<scene_uniform_description_t> renderer{
		glpp::object::shader_t(glpp::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::object::shader_t(glpp::object::shader_type_t::fragment, std::ifstream("fragment.glsl"))
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
// 			2, 2, // width, height
// 			{ // initializer list for the pixel data
// 				glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
// 				glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 1.0)
// 			}
		),
		glpp::object::image_format_t::prefered,
		glpp::object::clamp_mode_t::clamp_to_edge,
		glpp::object::filter_mode_t::linear
	);
	auto texture_unit_one = texture_one.bind_to_texture_slot();
	renderer.set_texture("texture_one" , texture_unit_one);

	glpp::render::view_t<vertex_description_t> view(
		model,
		&vertex_description_t::position,
		&vertex_description_t::tex
	);

	glpp::call(glClearColor, 0.2,0.2,0.2,1.0);
	window.enter_main_loop([&]() {
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
