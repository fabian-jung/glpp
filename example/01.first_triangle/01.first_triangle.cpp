#include <glpp/object/shader.hpp>
#include <glpp/render/renderer.hpp>
#include <glpp/render/model.hpp>
#include <glpp/render/view.hpp>
#include <glpp/system/window.hpp>

#include <glm/glm.hpp>

struct scene_uniform_description_t {
	glm::vec4 color;
};

struct vertex_shader_t : public glpp::object::shader_t {
	vertex_shader_t() :
		glpp::object::shader_t(
			glpp::object::shader_type_t::vertex,
			"#version 330 core\n\
			layout (location = 0) in vec3 aPos;\n\
			layout (location = 1) in vec3 color;\n\
			out vec3 c;\n\
			\n\
			void main()\n\
			{\n\
				c = color;\n\
				gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n\
			}"
		)
	{}
};

struct fragment_shader_t : public glpp::object::shader_t {
	fragment_shader_t() :
		glpp::object::shader_t(
			glpp::object::shader_type_t::fragment,
			"#version 330 core\n\
			uniform vec4 color;\n\
			out vec4 FragColor;\n\
			in vec3 c;\n\
			\n\
			void main()\n\
			{\n\
				FragColor = vec4(c,1)*color;\n\
			}"
		)
	{}
};

struct vertex_description_t {
	glm::vec3 position;
	glm::vec3 color;
};

class model_t : public glpp::render::model_t<vertex_description_t> {
public:
	void add_triangle(
		const glm::vec3& first,
		const glm::vec3& second,
		const glm::vec3& third
	) {
		m_verticies.reserve(m_verticies.size()+3);
		m_verticies.emplace_back(vertex_description_t{first, glm::vec3(1.0,0.0,0.0)});
		m_verticies.emplace_back(vertex_description_t{second, glm::vec3(0.0, 1.0,0.0)});
		m_verticies.emplace_back(vertex_description_t{third, glm::vec3(0.0,0.0,1.0)});
	}
};


int main(
	__attribute__((unused)) int argc,
	__attribute__((unused)) char **argv
) {

	glpp::system::window_t window(800, 600, "example", glpp::system::vsync_t::off);
	glpp::render::renderer_t<scene_uniform_description_t> renderer {
		vertex_shader_t(),
		fragment_shader_t()
	};
	renderer.set_uniform_name( &scene_uniform_description_t::color, "color");
	renderer.set_uniform( &scene_uniform_description_t::color, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	model_t model;
	model.add_triangle(
		glm::vec3{-1.0, -1.0, 0.0},
		glm::vec3{1.0, -1.0, 0.0},
		glm::vec3{0.0, 1.0, 0.0}
	);

	glpp::render::view_t<vertex_description_t> view(
		model,
		&vertex_description_t::position,
		&vertex_description_t::color
	);

	glpp::call(glClearColor, 0.5,0.0,0.0,1.0);
	window.enter_main_loop([&]() {
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});

	return 0;
}
