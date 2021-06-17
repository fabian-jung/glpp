#pragma once

#include "glpp/gl.hpp"
#include "buffer.hpp"
#include "attribute_properties.hpp"
#include <algorithm>
#include <stdexcept>

namespace glpp::core::object {

class vertex_array_t : public object_t<> {
public:

	vertex_array_t();

	vertex_array_t(vertex_array_t&& mov) = default;
	vertex_array_t(const vertex_array_t& cpy) = delete;

	vertex_array_t& operator=(vertex_array_t&& mov) = default;
	vertex_array_t& operator=(const vertex_array_t& cpy) = delete;
	
	void bind() const;

	template <class T>
	void bind_buffer(
		const buffer_t<T>& buffer,
		GLuint binding_point = 0,
		size_t stride = sizeof(T)
	);

	void attach_buffer(
		GLuint binding_point,
		GLuint index,
		GLuint elements_per_vertex,
		GLenum type,
		GLintptr offset = 0,
		bool normalized = false
	);

	template <class T>
	void attach(
		const buffer_t<T>& buffer,
		GLuint index,
		GLuint binding_point,
		size_t stride = 0,
		GLuint elements_per_vertex = attribute_properties<T>::elements_per_vertex,
		GLenum type = attribute_properties<T>::type,
		GLintptr offset = 0,
		bool normalized = false
	);

	template <class T>
	void attach(
		const buffer_t<T>& buffer,
		GLuint index,
		size_t stride = 0,
		GLuint elements_per_vertex = attribute_properties<T>::elements_per_vertex,
		GLenum type = attribute_properties<T>::type,
		GLintptr offset = 0,
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
void vertex_array_t::bind_buffer(
	const buffer_t<T>& buffer,
	GLuint binding_point,
	size_t stride
) {
	if(buffer.target() == buffer_target_t::element_array_buffer) {
		glVertexArrayElementBuffer(id(), buffer.id());
	} else {
		glVertexArrayVertexBuffer(id(), binding_point, buffer.id(), 0, stride);
	}
}

template <class T>
void vertex_array_t::attach(
	const buffer_t<T>& buffer,
	GLuint index,
	GLuint binding_point,
	size_t stride,
	GLuint elements_per_vertex,
	GLenum type,
	GLintptr offset,
	bool normalized
) {
	if(stride == 0) {
		stride = sizeof(T);
	}
	bind_buffer(buffer, binding_point, stride);
	attach_buffer(binding_point, index, elements_per_vertex, type, offset, normalized);
}

template <class T>
void vertex_array_t::attach(
	const buffer_t<T>& buffer,
	GLuint index,
	size_t stride,
	GLuint elements_per_vertex,
	GLenum type,
	GLintptr offset,
	bool normalized
) {
	attach(buffer, index, index, stride, elements_per_vertex, type, offset, normalized);
}

}
