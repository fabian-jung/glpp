#pragma once

#include <type_traits>
#include <glpp/object/buffer.hpp>
#include <glpp/object/vertex_array.hpp>

namespace glpp::render {

template <class uniform_description_t>
class renderer_t;

enum class view_primitives_t : GLenum {
	points = GL_POINTS,
	line_strip = GL_LINE_STRIP,
	line_loop = GL_LINE_LOOP,
	lines = GL_LINES,
	line_strip_adjacency = GL_LINE_STRIP_ADJACENCY,
	lines_adjacency = GL_LINES_ADJACENCY,
	triangle_strip = GL_TRIANGLE_STRIP,
	triangle_fan = GL_TRIANGLE_FAN,
	triangles = GL_TRIANGLES,
	triangle_strip_adjacency = GL_TRIANGLE_STRIP_ADJACENCY,
	triangles_adjacency = GL_TRIANGLES_ADJACENCY,
	patches = GL_PATCHES
};

template <class Attribute_Description, view_primitives_t primitive = view_primitives_t::triangles>
class view_t {
public:
	using attribute_description_t = Attribute_Description;

	template <
		class model_t,
		std::enable_if_t<std::is_same_v<typename model_t::attribute_description_t, attribute_description_t> ,int> = 0,
		class... T
	>
	explicit view_t(const model_t& model, T attribute_description_t::* ...attributes) :
		m_vao(),
		m_size(model.size()),
		m_buffer(
			glpp::object::buffer_target_t::array_buffer,
			model.data(),
			model.size()*sizeof(attribute_description_t),
			glpp::object::buffer_usage_t::static_draw
		)
	{
		size_t index = 0;
		(m_vao.attach(
			m_buffer,
			index++,
			sizeof(attribute_description_t)/*-sizeof(T)*/,
			glpp::object::attribute_properties<T>::elements_per_vertex,
			glpp::object::attribute_properties<T>::type,
			offset(attributes)
		), ...);
	}

private:
	template <class uniform_description_t>
	friend class renderer_t;

	void draw() const;
	void draw_instanced(size_t count) const;

	template <class T>
	static constexpr void* offset(T attribute_description_t::* attr);

	glpp::object::vertex_array_t m_vao;
	size_t m_size;
	glpp::object::buffer_t<attribute_description_t> m_buffer;
};

/*
 * Implementation
 */

template <class Attribute_Description, view_primitives_t primitive>
void view_t<Attribute_Description, primitive>::draw() const {
	m_vao.bind();
	glpp::call(glDrawArrays, static_cast<GLenum>(primitive), 0, m_size);
}

template <class Attribute_Description, view_primitives_t primitive>
void view_t<Attribute_Description, primitive>::draw_instanced(size_t count) const {
	m_vao.bind();
	glpp::call(glDrawArraysInstanced, static_cast<GLenum>(primitive), 0, m_size, count);
}

template <class Attribute_Description, view_primitives_t primitive>
template <class T>
constexpr void* view_t<Attribute_Description, primitive>::offset(T attribute_description_t::* attr) {
	return reinterpret_cast<void*>(
		&(reinterpret_cast<attribute_description_t*>(0)->*attr)
	);
}

}
