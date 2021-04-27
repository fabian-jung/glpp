#pragma once

#include "glpp/core/glpp.hpp"
#include <glm/glm.hpp>

namespace glpp::core::object {

template <class T>
struct attribute_properties;

template <>
struct attribute_properties<GLfloat> {
	using value_type = GLfloat;
	static constexpr GLenum type = GL_FLOAT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLdouble> {
	using value_type = GLdouble;
	static constexpr GLenum type = GL_DOUBLE;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLint> {
	using value_type = GLint;
	static constexpr GLenum type = GL_INT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLuint> {
	using value_type = GLint;
	static constexpr GLenum type = GL_UNSIGNED_INT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLshort> {
	using value_type = GLshort;
	static constexpr GLenum type = GL_SHORT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLushort> {
	using value_type = GLushort;
	static constexpr GLenum type = GL_UNSIGNED_SHORT;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLbyte> {
	using value_type = GLbyte;
	static constexpr GLenum type = GL_BYTE;
	static constexpr size_t elements_per_vertex = 1;
};

template <>
struct attribute_properties<GLubyte> {
	using value_type = GLubyte;
	static constexpr GLenum type = GL_UNSIGNED_BYTE;
	static constexpr size_t elements_per_vertex = 1;
};

template <class T>
struct attribute_properties<glm::tvec2<T>> {
	using value_type = typename attribute_properties<T>::value_type;
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 2;
};

template <class T>
struct attribute_properties<glm::tvec3<T>> {
	using value_type = typename attribute_properties<T>::value_type;
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 3;
};

template <class T>
struct attribute_properties<glm::tvec4<T>> {
	using value_type = typename attribute_properties<T>::value_type;
	static constexpr GLenum type = attribute_properties<T>::type;
	static constexpr size_t elements_per_vertex = 4;
};

}
