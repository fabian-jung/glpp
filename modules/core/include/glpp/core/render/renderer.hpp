#pragma once

#include <string>
#include <unordered_map>
#include <glpp/core/object/texture_atlas.hpp>
#include <glpp/core/object/shader.hpp>

namespace glpp::core::render {

namespace detail {
	struct none_t {};

	template <class T, class Y>
	constexpr ptrdiff_t get_offset(Y T::* ptr) {
		return reinterpret_cast<char*>(&(reinterpret_cast<T*>(0)->*ptr))-reinterpret_cast<char*>(0);
	}
}

template <class uniform_description_t = detail::none_t>
class renderer_t {
public:
	template <class... shader_t>
	renderer_t(
		const shader_t&... shaders
	);

	template <class view_t>
	void render(const view_t& view);

	template <class view_t>
	void render_instanced(const view_t& view, size_t count);

	template <class T>
	void set_uniform_name(T uniform_description_t::* uniform, std::string name);

	template <class T>
	void set_uniform(T uniform_description_t::* uniform, const T& value);

	template <class T>
	void set_uniform_array(T uniform_description_t::* uniform, const T* value, const size_t size);

	void set_texture(const char* name, const object::texture_slot_t& texture_slot);
	void set_texture(const char* name, object::texture_slot_t&& texture_slot);

	template <class texture_slot_iterator>
	void set_texture_array(const char* name, texture_slot_iterator begin, texture_slot_iterator end);

	template <class Container>
	void set_texture_array(const char* name, const Container& container);

	template <class AllocPolicy>
	void set_texture_atlas(const char* name, const object::texture_atlas_slot_t<AllocPolicy>& texture_atlas);

private:
	glpp::core::object::shader_program_t m_shader;
	std::unordered_map<size_t, GLint> m_uniform_map;
	std::unordered_map<std::string, object::texture_slot_t> m_texture_slots;
};

/**
 * Implementation
 */

template <class uniform_description_t>
template <class... shader_t>
renderer_t<uniform_description_t>::renderer_t(
	const shader_t&... shaders
) :
	m_shader(shaders...)
{

}

template <class uniform_description_t>
template <class view_t>
void renderer_t<uniform_description_t>::render(const view_t& view) {
	m_shader.use();
	view.draw();
}

template <class uniform_description_t>
template <class view_t>
void renderer_t<uniform_description_t>::render_instanced(const view_t& view, size_t count) {
	m_shader.use();
	view.draw_instanced(count);
}


template <class uniform_description_t>
template <class T>
void renderer_t<uniform_description_t>::set_uniform_name(T uniform_description_t::* uniform, std::string name) {
	m_uniform_map[detail::get_offset(uniform)] = m_shader.uniform_location(name.c_str());
}

template <class uniform_description_t>
template <class T>
void renderer_t<uniform_description_t>::set_uniform(T uniform_description_t::* uniform, const T& value) {
	const auto& location = m_uniform_map[detail::get_offset(uniform)];
	m_shader.set_uniform(location, value);
}

template <class uniform_description_t>
template <class T>
void renderer_t<uniform_description_t>::set_uniform_array(T uniform_description_t::* uniform, const T* value, const size_t size) {
	const auto& location = m_uniform_map[detail::get_offset(uniform)];
	m_shader.set_uniform_array(location, value, size);
}

template <class uniform_description_t>
void renderer_t<uniform_description_t>::set_texture(const char* name, const object::texture_slot_t& texture_slot) {
	m_shader.set_texture(name, texture_slot);
}

template <class uniform_description_t>
void renderer_t<uniform_description_t>::set_texture(const char* name, object::texture_slot_t&& texture_slot) {
	m_shader.set_texture(name, texture_slot);
	m_texture_slots[name]=std::move(texture_slot);
}

template <class uniform_description_t>
template <class texture_slot_iterator>
void renderer_t<uniform_description_t>::set_texture_array(const char* name, texture_slot_iterator begin, texture_slot_iterator end) {
	m_shader.set_texture_array(name, begin, end);
}

template <class uniform_description_t>
template <class Container>
void renderer_t<uniform_description_t>::set_texture_array(const char* name, const Container& container) {
	set_texture_array(name, container.begin(), container.end());
}

template <class uniform_description_t>
template <class AllocPolicy>
void renderer_t<uniform_description_t>::set_texture_atlas(const char* name, const object::texture_atlas_slot_t<AllocPolicy>& texture_atlas_slots) {
	set_texture_array(name, texture_atlas_slots.begin(), texture_atlas_slots.end());
}

}
