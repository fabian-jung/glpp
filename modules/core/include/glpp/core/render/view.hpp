#pragma once

#include <type_traits>
#include <glpp/core/object/buffer.hpp>
#include <glpp/core/object/vertex_array.hpp>
#include <glpp/core/render/model.hpp>

namespace glpp::core::render {

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
			glpp::core::object::buffer_target_t::array_buffer,
			model_traits<model_t>::verticies(model).data(),
			model_traits<model_t>::verticies(model).size()*sizeof(attribute_description_t),
			glpp::core::object::buffer_usage_t::static_draw
		)
	{
		static_assert(sizeof...(T) > 0, "Trying to initialise view without attributes");
		size_t index = 0;
		constexpr auto binding = 0u;
		m_vao.bind_buffer(m_buffer, binding);
		(m_vao.attach_buffer(
			binding,
			index++,
			glpp::core::object::attribute_properties<T>::elements_per_vertex,
			glpp::core::object::attribute_properties<T>::type,
			offset(attributes)
		), ...);
		if constexpr(model_traits_t::instanced()) {
			m_vao.bind(); // TODO
			m_vao.bind(); 
			m_indicies = glpp::core::object::buffer_t<GLuint>(
				glpp::core::object::buffer_target_t::element_array_buffer,
				model_traits<model_t>::indicies(model).data(),
					model_traits<model_t>::indicies(model).size()*sizeof(GLuint), //TODO  GLint
					glpp::core::object::buffer_usage_t::static_draw
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
	static constexpr GLintptr offset(T attribute_description_t::* attr);

	glpp::core::object::vertex_array_t m_vao;
	size_t m_size;
	glpp::core::object::buffer_t<attribute_description_t> m_buffer;
	glpp::core::object::buffer_t<GLuint> m_indicies;
};

template <class Model, class... T>
view_t(const Model& model, T model_traits<Model>::attribute_description_t::* ...attributes) -> view_t<Model>;

/*
 * Implementation
 */

template <class Model, view_primitives_t primitive>
void view_t<Model, primitive>::draw() const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		glDrawElements(static_cast<GLenum>(primitive), m_size, GL_UNSIGNED_INT, 0);
	} else {
		glDrawArrays(static_cast<GLenum>(primitive), 0, m_size);
	}
}

template <class Model, view_primitives_t primitive>
void view_t<Model, primitive>::draw_instanced(size_t count) const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		glDrawElementsInstanced(static_cast<GLenum>(primitive), m_size, GL_UNSIGNED_INT, 0, count);
	} else {
		glDrawArraysInstanced(static_cast<GLenum>(primitive), 0, m_size, count);
	}
}

template <class Model, view_primitives_t primitive>
template <class T>
constexpr GLintptr view_t<Model, primitive>::offset(T attribute_description_t::* attr) {
	return reinterpret_cast<GLintptr>(
		&(reinterpret_cast<attribute_description_t*>(0)->*attr)
	);
}

}
