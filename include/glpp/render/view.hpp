#pragma once

#include <type_traits>
#include <glpp/object/buffer.hpp>
#include <glpp/object/vertex_array.hpp>
#include <glpp/render/model.hpp>

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

template <class Model, view_primitives_t primitive = view_primitives_t::triangles>
class view_t {
public:
	using model_traits_t = model_traits<Model>;
	using attribute_description_t = typename model_traits_t::attribute_description_t;

	template <
		class model_t,
		class... T
	>
	explicit view_t(const model_t& model, T attribute_description_t::* ...attributes) :
		m_vao(),
		m_size(
			[&]() -> size_t {
				if constexpr(model_traits<model_t>::instanced()) {
					return model_traits<model_t>::indicies(model).size();
				} else {
					return model_traits<model_t>::verticies(model).size();
				}
			}()
		),
		m_buffer(
			glpp::object::buffer_target_t::array_buffer,
			model_traits<model_t>::verticies(model).data(),
			model_traits<model_t>::verticies(model).size()*sizeof(attribute_description_t),
			glpp::object::buffer_usage_t::static_draw
		)
	{
		static_assert(sizeof...(T) > 0, "Trying to initialise view without attributes");
		size_t index = 0;
		(m_vao.attach(
			m_buffer,
			index++,
			sizeof(attribute_description_t)/*-sizeof(T)*/,
			glpp::object::attribute_properties<T>::elements_per_vertex,
			glpp::object::attribute_properties<T>::type,
			offset(attributes)
		), ...);
		if constexpr(model_traits_t::instanced()) {
			m_vao.bind(); // TODO
			m_indicies = glpp::object::buffer_t<GLuint>(
				glpp::object::buffer_target_t::element_array_buffer,
				model_traits<model_t>::indicies(model).data(),
					model_traits<model_t>::indicies(model).size()*sizeof(GLuint), //TODO  GLint
					glpp::object::buffer_usage_t::static_draw
			);
			m_indicies.bind();
		}
	}

	view_t(view_t&& mov) = default;
	view_t& operator=(view_t&&) = default;

	view_t(const view_t& mov) = delete;
	view_t& operator=(const view_t&) = delete;

	auto size() const {
		return m_size;
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
	glpp::object::buffer_t<GLuint> m_indicies;
};

template <class Model, class... T>
view_t(const Model& model, T model_traits<Model>::attribute_description_t::* ...attributes) -> view_t<Model>;

/*
 * Implementation
 */

template <class Attribute_Description, view_primitives_t primitive>
void view_t<Attribute_Description, primitive>::draw() const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		glDrawElements(static_cast<GLenum>(primitive), m_size, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(static_cast<GLenum>(primitive), 0, m_size);
	}
}

template <class Attribute_Description, view_primitives_t primitive>
void view_t<Attribute_Description, primitive>::draw_instanced(size_t count) const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		glDrawArraysInstanced(static_cast<GLenum>(primitive), 0, m_size, count);
	} else {
		glDrawElementsInstanced(static_cast<GLenum>(primitive), m_size, GL_UNSIGNED_INT, 0, count);
	}
}

template <class Attribute_Description, view_primitives_t primitive>
template <class T>
constexpr void* view_t<Attribute_Description, primitive>::offset(T attribute_description_t::* attr) {
	return reinterpret_cast<void*>(
		&(reinterpret_cast<attribute_description_t*>(0)->*attr)
	);
}

}
