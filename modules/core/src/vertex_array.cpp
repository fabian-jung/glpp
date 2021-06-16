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

void vertex_array_t::attach_buffer(
	GLuint binding_point,
	GLuint index,
	GLuint elements_per_vertex,
	GLenum type,
	GLintptr offset,
	bool normalized
) {
	glEnableVertexArrayAttrib(id(), index);
	glVertexArrayAttribBinding(id(), index, binding_point);
	constexpr std::array integral_types = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT};
	if(type == GL_DOUBLE) {
		glVertexArrayAttribLFormat(id(), index, elements_per_vertex, type, offset);
	} else if(std::any_of(integral_types.begin(), integral_types.end(), [type](GLenum v){ return type == v; })){
		glVertexArrayAttribIFormat(id(), index, elements_per_vertex, type, offset);
	} else {
		glVertexArrayAttribFormat(id(), index, elements_per_vertex, type, normalized, offset);
	}
}

GLuint vertex_array_t::create() {
	GLuint id;
	glCreateVertexArrays(1, &id);
	return id;
}

void vertex_array_t::destroy(GLuint id) {
	glDeleteVertexArrays(1, &id);
}

}
