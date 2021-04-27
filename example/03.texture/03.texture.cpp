#include <glpp/system/window.hpp>
#include <glpp/core/render/renderer.hpp>
#include <glpp/core/render/model.hpp>
#include <glpp/core/render/view.hpp>
#include <glpp/core/object/texture.hpp>

#include <fstream>

struct scene_uniform_description_t{
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
	struct model_traits<::model_t> : public model_traits<glpp::core::render::model_t<vertex_description_t>> {};
}

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[]) {
	glpp::system::window_t window(800, 600, "texture");
	window.set_input_mode(glpp::system::input_mode_t::wait);

	glpp::core::render::renderer_t<scene_uniform_description_t> renderer{
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::vertex, std::ifstream("vertex.glsl")),
		glpp::core::object::shader_t(glpp::core::object::shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};

	model_t model;
	model.add_quad(
		vertex_description_t{{-0.75, -0.75}, {0, 0}},
		vertex_description_t{{ 0.75, -0.75}, {1, 0}},
		vertex_description_t{{ 0.75,  0.75}, {1, 1}},
		vertex_description_t{{-0.75,  0.75}, {0, 1}}
	);



	glpp::core::object::texture_t texture_one(
		glpp::core::object::image_t<glm::vec3>(
			"smiley.png"
// 			2, 2, // width, height
// 			{ // initializer list for the pixel data
// 				glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0),
// 				glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 1.0, 1.0)
// 			}
		),
		glpp::core::object::image_format_t::prefered,
		glpp::core::object::clamp_mode_t::clamp_to_edge,
		glpp::core::object::filter_mode_t::linear
	);
	auto texture_unit_one = texture_one.bind_to_texture_slot();
	renderer.set_texture("texture_one" , texture_unit_one);

	glpp::core::render::view_t view(
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
