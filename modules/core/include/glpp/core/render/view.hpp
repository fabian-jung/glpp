#pragma once

#include <type_traits>
#include <boost/pfr.hpp>
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


template <class Model>
struct view_base_t {
	glpp::core::object::vertex_array_t m_vao;
	void m_indicies() {};

	explicit view_base_t(const Model&) noexcept {};
};

template <InstancedModel Model>
struct view_base_t<Model> {
	using m_traits =  model_traits<Model>;
	using index_t = typename m_traits::index_t;

	glpp::core::object::vertex_array_t m_vao;
	glpp::core::object::buffer_t<index_t> m_indicies;
	
	explicit view_base_t(const Model& model) :
		m_indicies(
			glpp::core::object::buffer_target_t::element_array_buffer,
			m_traits::indicies(model).data(),
			m_traits::indicies(model).size()*sizeof(index_t),
			glpp::core::object::buffer_usage_t::static_draw
		)
	{
		m_vao.bind_buffer(m_indicies);
	}

};

template <class Model, view_primitives_t primitive = view_primitives_t::triangles>
class view_t : private view_base_t<Model> {
public:
	using model_traits_t = model_traits<Model>;
	using attribute_description_t = typename model_traits_t::attribute_description_t;

	template <
		class model_t,
		class... T
	>
	explicit view_t(const model_t& model, T attribute_description_t::* ...attributes);

	view_t(view_t&& mov) noexcept = default;
	view_t& operator=(view_t&& mov) noexcept = default;

	view_t(const view_t& cpy) = delete;
	view_t& operator=(const view_t& cpy) = delete;

	auto size() const {
		return m_size;
	}

private:
	using view_base_t<Model>::m_vao;
	using view_base_t<Model>::m_indicies;

	template <class uniform_description_t>
	friend class renderer_t;

	void draw() const;
	void draw_instanced(size_t count) const;

	template <class... T>
	void attatch_selected_buffers(T attribute_description_t::* ...attributes);

	template <size_t... Index>
	void attatch_all_buffers(std::index_sequence<Index...>);

	template <class T>
	static constexpr GLintptr offset(T attribute_description_t::* attr);

	template <size_t N>
	static constexpr GLintptr offset();

	size_t m_size;
	glpp::core::object::buffer_t<attribute_description_t> m_buffer;
};

template <class Model, class... T>
view_t(const Model& model, T model_traits<Model>::attribute_description_t::* ...attributes) -> view_t<Model>;

/*
 * Implementation
 */

template <class Model, view_primitives_t primitive>
template <class model_t, class... T>
view_t<Model, primitive>::view_t(const model_t& model, T attribute_description_t::* ...attributes) :
	view_base_t<Model>(model),
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
	if constexpr(sizeof...(T) > 0) {
		attatch_selected_buffers(attributes...);
	} else {
		constexpr auto number_of_attributes = boost::pfr::tuple_size_v<attribute_description_t>;
		static_assert(number_of_attributes > 0, "The model attribute_description_t must have at least one member");
		attatch_all_buffers(std::make_index_sequence<number_of_attributes>());
	}
}

template <class Model, view_primitives_t primitive>
void view_t<Model, primitive>::draw() const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		using index_t = typename model_traits<Model>::index_t;
		constexpr auto index_enum = object::attribute_properties<index_t>::type;
		static_assert(
			index_enum == GL_UNSIGNED_BYTE ||
			index_enum == GL_UNSIGNED_SHORT ||
			index_enum ==  GL_UNSIGNED_INT,
			"Index type is required to be ubyte, ushort or uint."
		);
		glDrawElements(static_cast<GLenum>(primitive), m_size, index_enum, 0);
	} else {
		glDrawArrays(static_cast<GLenum>(primitive), 0, m_size);
	}
}

template <class Model, view_primitives_t primitive>
void view_t<Model, primitive>::draw_instanced(size_t count) const {
	m_vao.bind();
	if constexpr(model_traits_t::instanced()) {
		using index_t = typename model_traits<Model>::index_t;
		constexpr auto index_enum = object::attribute_properties<index_t>::type;
		static_assert(
			index_enum == GL_UNSIGNED_BYTE ||
			index_enum == GL_UNSIGNED_SHORT ||
			index_enum ==  GL_UNSIGNED_INT,
			"Index type is required to be ubyte, ushort or uint."
		);
		glDrawElementsInstanced(static_cast<GLenum>(primitive), m_size, index_enum, 0, count);
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

template <class Model, view_primitives_t primitive>
template <size_t N>
constexpr GLintptr view_t<Model, primitive>::offset() {
	constexpr attribute_description_t attrib {};

	return 
		reinterpret_cast<GLintptr>(
			&(boost::pfr::get<N>(attrib))
		) -
		reinterpret_cast<GLintptr>(&attrib);
}

template <class Model, view_primitives_t primitive>
template <class... T>
void view_t<Model, primitive>::attatch_selected_buffers(T attribute_description_t::* ...attributes) {
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
}

template <class Model, view_primitives_t primitive>
template <size_t... Index>
void view_t<Model, primitive>::attatch_all_buffers(std::index_sequence<Index...>) {
	static_assert(sizeof...(Index) > 0, "At leaset one buffer must be attatched.");
	constexpr auto binding = 0u;
	m_vao.bind_buffer(m_buffer, binding);
	(m_vao.attach_buffer(
		binding,
		Index,
		glpp::core::object::attribute_properties<decltype(boost::pfr::get<Index>(attribute_description_t{}))>::elements_per_vertex,
		glpp::core::object::attribute_properties<decltype(boost::pfr::get<Index>(attribute_description_t{}))>::type,
		offset<Index>()
	), ...);
}

}
