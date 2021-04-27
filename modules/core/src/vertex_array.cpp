#include "glpp/core/object/vertex_array.hpp"

namespace glpp::core::object {

vertex_array_t::vertex_array_t() :
	object_t(
		create(),
		destroy
	)
{}

void vertex_array_t::bind() const {
	glBindVertexArray(id());
}

GLuint vertex_array_t::create() {
	GLuint id;
	glGenVertexArrays(1, &id);
	return id;
}

void vertex_array_t::destroy(GLuint id) {
	glDeleteVertexArrays(1, &id);
}

}
