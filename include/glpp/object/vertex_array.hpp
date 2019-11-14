#pragma once

#include "glpp/glpp.hpp"
#include "buffer.hpp"
#include "attribute_properties.hpp"

namespace glpp::object {

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
	call(glEnableVertexAttribArray, index);
	call(glVertexAttribPointer, index, elements_per_vertex, type, normalized, stride, offset);
}

}
