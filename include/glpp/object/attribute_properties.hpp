#pragma once

#include "glpp/glpp.hpp"
#include <glm/glm.hpp>

namespace glpp::object {

template <class T>
struct attribute_properties;

template <>
struct attribute_properties<GLfloat> {
	static constexpr GLenum type = GL_FLOAT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLdouble> {
	static constexpr GLenum type = GL_DOUBLE;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLint> {
	static constexpr GLenum type = GL_INT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLuint> {
	static constexpr GLenum type = GL_UNSIGNED_INT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLshort> {
	static constexpr GLenum type = GL_SHORT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLushort> {
	static constexpr GLenum type = GL_UNSIGNED_SHORT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLbyte> {
	static constexpr GLenum type = GL_BYTE;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLubyte> {
	static constexpr GLenum type = GL_UNSIGNED_BYTE;
	static constexpr size_t elements_per_vertex = 1;
};

template <class T>
struct attribute_properties<glm::tvec2<T>> {
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 2;
};

template <class T>
struct attribute_properties<glm::tvec3<T>> {
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 3;
};

template <class T>
struct attribute_properties<glm::tvec4<T>> {
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 4;
};




}
