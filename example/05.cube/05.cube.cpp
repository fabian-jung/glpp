#include <glpp/core.hpp>
#include <glpp/system.hpp>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

using namespace glpp::system;
using namespace glpp::object;
using namespace glpp::render;

struct scene_uniform_description_t {
	glm::mat4 mvp;
};

struct vertex_description_t {
	glm::vec3 position;
	glm::vec3 norm;
};

class cube_model_t : public glpp::render::model_t<vertex_description_t> {
public:
	cube_model_t(std::initializer_list<std::pair<glm::vec3, glm::vec3>> list) {
		for(const auto& p : list) {
			add_cube(p.first, p.second);
		}
	}

	void add_cube(glm::vec3 center, glm::vec3 size) {
		const auto offset = 0.5f*size;
		struct base_t {
			glm::vec3 base_one;
			glm::vec3 base_two;
			glm::vec3 normal;
		};

		constexpr std::array<base_t, 6> base{{
			{{1,0,0}, {0,1,0}, {0,0,1}},
			{{1,0,0}, {0,1,0}, {0,0,-1}},
			{{0,1,0}, {0,0,1}, {1,0,0}},
			{{0,1,0}, {0,0,1}, {-1,0,0}},
			{{1,0,0}, {0,0,1}, {0,1,0}},
			{{1,0,0}, {0,0,1}, {0,-1,0}}
		}};
		for(const auto& b : base) {
			add_quad(
				{center+offset*(-b.base_one-b.base_two+b.normal), b.normal},
				{center+offset*( b.base_one-b.base_two+b.normal), b.normal},
				{center+offset*( b.base_one+b.base_two+b.normal), b.normal},
				{center+offset*(-b.base_one+b.base_two+b.normal), b.normal}
			);
		}
	}

private:
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

int main(
	__attribute__((unused)) int argc,
	__attribute__((unused)) char *argv[]
) {
	window_t window(800, 600, "05.framebuffer", glpp::system::vsync_t::off);
	window.input_handler().set_keyboard_action(
		glpp::system::key_t::escape,
		action_t::press,
		[&](int){
			window.close();
		}
	);

	renderer_t<scene_uniform_description_t> renderer {
		shader_t(shader_type_t::vertex, std::ifstream("vertex.glsl")),
		shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};
	renderer.set_uniform_name(&scene_uniform_description_t::mvp, "mvp");

	view_t<vertex_description_t> view(
		cube_model_t{{std::make_pair(glm::vec3{0,0,0}, glm::vec3{2, 2, 2})}},
		&vertex_description_t::position,
		&vertex_description_t::norm
	);

	glpp::call(glClearColor, 0.2,0.2,0.2,1.0);
	constexpr float fov = 45;
	constexpr float z_near = 1.0;
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

	window.enter_main_loop([&]() {
		m = glm::rotate(m, glm::radians(0.01f), glm::vec3(0,1,0));
		const auto mvp = p*v*m;
		renderer.set_uniform(&scene_uniform_description_t::mvp, glm::transpose(mvp));

		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
