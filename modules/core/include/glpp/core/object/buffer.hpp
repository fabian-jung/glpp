#pragma once

#include "glpp/core/object.hpp"
#include "glpp/gl/constants.hpp"
#include "glpp/gl/functions.hpp"
#include <vector>

namespace glpp::core::object {

enum class buffer_target_t : GLenum {
	array_buffer = GL_ARRAY_BUFFER,
	atomic_counter_buffer = GL_ATOMIC_COUNTER_BUFFER,
	copy_read_buffer = GL_COPY_READ_BUFFER,
	copy_write_buffer = GL_COPY_WRITE_BUFFER,
	dispatch_indirect_buffer = GL_DISPATCH_INDIRECT_BUFFER,
	draw_indirect_buffer = GL_DRAW_INDIRECT_BUFFER,
	element_array_buffer = GL_ELEMENT_ARRAY_BUFFER,
	pixel_pack_buffer = GL_PIXEL_PACK_BUFFER,
	pixel_unpack_buffer = GL_PIXEL_UNPACK_BUFFER,
	query_buffer = GL_QUERY_BUFFER,
	shader_storage_buffer = GL_SHADER_STORAGE_BUFFER,
	texture_buffer = GL_TEXTURE_BUFFER,
	transform_feedback_buffer = GL_TRANSFORM_FEEDBACK_BUFFER,
	uniform_buffer = GL_UNIFORM_BUFFER
};

enum class buffer_usage_t : GLenum {
	stream_draw = GL_STREAM_DRAW,
	stream_read = GL_STREAM_READ,
	stream_copy = GL_STREAM_COPY,
	static_draw = GL_STATIC_DRAW,
	static_read = GL_STATIC_READ,
	static_copy = GL_STATIC_COPY,
	dynamic_draw = GL_DYNAMIC_DRAW,
	dynamic_read = GL_DYNAMIC_READ,
	dynamic_copy = GL_DYNAMIC_COPY
};

template <class T = float>
class buffer_t : public object_t<> {
public:

	buffer_t();

	buffer_t(const buffer_t& cpy) = delete;
	buffer_t(buffer_t&& mov) noexcept = default;
	
	buffer_t& operator=(const buffer_t& cpy) = delete;
	buffer_t& operator=(buffer_t&& mov) noexcept = default;
	
	buffer_t(buffer_target_t target, const T* data, size_t size, buffer_usage_t usage);

	void bind() const;

	std::vector<T> read() const;
	void read(T* data) const;

	size_t size() const ;

	buffer_target_t target() const;

private:
	GLuint create();
	static void destroy(GLuint id);

	buffer_target_t m_target;
	size_t m_size = 0;
};

/*
 * Implementation
 */

template <class T>
buffer_t(buffer_target_t, const T*, size_t, buffer_usage_t) -> buffer_t<T>;

template <class T>
buffer_t<T>::buffer_t() :
	object_t(
		create(),
		destroy
	)
{}

template <class T>
buffer_t<T>::buffer_t(buffer_target_t target, const T* data, size_t size, buffer_usage_t usage) :
	object_t(
		create(),
		destroy
	),
	m_target(target),
	m_size(size)
{
	glNamedBufferData(
		id(),
		size,
		data,
		static_cast<GLenum>(usage)
	);
}

template <class T>
void buffer_t<T>::bind() const {
	glBindBuffer(static_cast<GLenum>(m_target), id());
}

template <class T>
GLuint buffer_t<T>::create() {
	GLuint id;
	glCreateBuffers(1, &id);
	return id;
}

template <class T>
void buffer_t<T>::destroy(GLuint id) {
	glDeleteBuffers(1, &id);
}

template <class T>
std::vector<T> buffer_t<T>::read() const {
	std::vector<T> result;
	result.resize(m_size/sizeof(T));
	read(result.data());
	return result;
}

template <class T>
void buffer_t<T>::read(T* data) const {
	glGetNamedBufferSubData(id(), 0, m_size, data);
}

template <class T>
size_t buffer_t<T>::size() const {
	return m_size;
}

template <class T>
buffer_target_t buffer_t<T>::target() const {
	return m_target;
}


}
