#pragma once

#include <string>
#include <unordered_map>
#include <glpp/object/shader.hpp>

namespace glpp::render {

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

	template <class T>
	void set_uniform_name(T uniform_description_t::* uniform, std::string name);

	template <class T>
	void set_uniform(T uniform_description_t::* uniform, const T& value);

	void set_texture(const char* name, const object::texture_slot_t& texture_slot);

private:
	glpp::object::shader_program_t m_shader;
	std::unordered_map<size_t, std::string> m_uniform_map;
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
template <class T>
void renderer_t<uniform_description_t>::set_uniform_name(T uniform_description_t::* uniform, std::string name) {
	m_uniform_map[detail::get_offset(uniform)] = std::move(name);
}

template <class uniform_description_t>
template <class T>
void renderer_t<uniform_description_t>::set_uniform(T uniform_description_t::* uniform, const T& value) {
	const auto& name = m_uniform_map[detail::get_offset(uniform)];
	m_shader.set_uniform(name.c_str(), value);
}

template <class uniform_description_t>
void renderer_t<uniform_description_t>::set_texture(const char* name, const object::texture_slot_t& texture_slot) {
	m_shader.set_texture(name, texture_slot);
}

}
