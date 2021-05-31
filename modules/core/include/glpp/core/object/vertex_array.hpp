#pragma once

#include "glpp/gl.hpp"
#include "buffer.hpp"
#include "attribute_properties.hpp"
#include <algorithm>

namespace glpp::core::object {

class vertex_array_t : public object_t<> {
public:

	vertex_array_t();

	void bind() const;

	template <class T>
	void attach(
		const buffer_t<T>& buffer,
		GLuint index,
		size_t stride = 0,
		GLuint elements_per_vertex = attribute_properties<T>::elements_per_vertex,
		GLenum type = attribute_properties<T>::type,
		void* offset = nullptr,
		bool normalized = false
	);

private:
	GLuint create();
	static void destroy(GLuint id);

};

/*
 * Implementation
 */

template <class T>
void vertex_array_t::attach(
	const buffer_t<T>& buffer,
	GLuint index,
	size_t stride,
	GLuint elements_per_vertex,
	GLenum type,
	void* offset,
	bool normalized
) {
	bind();
	buffer.bind();
	glEnableVertexAttribArray(index);
	constexpr std::array integral_types = {GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT};
	if(type == GL_DOUBLE) {
		glVertexAttribLPointer(index, elements_per_vertex, type, stride, offset);
	} else if(std::any_of(integral_types.begin(), integral_types.end(), [type](GLenum v){ return type == v; })){
		glVertexAttribIPointer(index, elements_per_vertex, type, stride, offset);
	} else {
		glVertexAttribPointer(index, elements_per_vertex, type, normalized, stride, offset);
	}
}

}
