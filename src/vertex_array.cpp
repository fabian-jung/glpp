#include "glpp/object/vertex_array.hpp"

namespace glpp::object {

vertex_array_t::vertex_array_t() :
	object_t(
		create(),
		destroy
	)
{}

void vertex_array_t::bind() const {
	call(glBindVertexArray, id());
}

GLuint vertex_array_t::create() {
	GLuint id;
	call(glGenVertexArrays,1, &id);
	return id;
}

void vertex_array_t::destroy(GLuint id) {
	call(glDeleteVertexArrays, 1, &id);
}

}
