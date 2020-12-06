#pragma once

#include "glpp/glpp.hpp"
#include "glpp/object.hpp"
#include "glpp/object/texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <istream>

namespace glpp::object {

enum class shader_type_t : GLenum {
	compute = GL_COMPUTE_SHADER,
	vertex = GL_VERTEX_SHADER,
	tess_control = GL_TESS_CONTROL_SHADER,
	tess_eval = GL_TESS_EVALUATION_SHADER,
	geometry = GL_GEOMETRY_SHADER,
	fragment = GL_FRAGMENT_SHADER
};

class shader_t : public object_t<> {
public:
	shader_t(shader_type_t type, const std::string& code);
	shader_t(shader_type_t type, std::istream& code);
	shader_t(shader_type_t type, std::istream&& code);

private:
	void init(const std::string& code);
	static void destroy(GLuint id);
};

class shader_program_t : public object_t<> {
public:

	template <class... shader_t>
	explicit shader_program_t(const shader_t&... shader);

	template <class Value>
	void set_uniform(const char* name, const Value& value);

	template <class Value>
	void set_uniform_array(const char* name, const Value* begin, const size_t size);

	void set_texture(const char* name, const texture_slot_t& texture);

	template <class texture_slot_iterator>
	void set_texture_array(const char* name, const texture_slot_iterator begin, const texture_slot_iterator end);

	void use();
private:

	class uniform_setter_t {
	public:
		uniform_setter_t(GLint location);

		template <class T>
		void set_value(const T& value);

		template <class T>
		void set_value(const T* begin, const size_t size);

	private:
		GLint location;
	};

	void attatch(const shader_t& shader);
	void link();
	static void destroy(GLuint id);
};

/*
 * Implementation
 */

template <class... shader_t>
shader_program_t::shader_program_t(const shader_t&... shader) :
	object_t(
		glCreateProgram(),
		destroy
	)
{
	if constexpr(sizeof...(shader_t) > 0) {
		(attatch(shader), ...);
		link();
		int success;
		char info_log[512];
		glGetProgramiv(id(), GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(id(), sizeof(info_log), NULL, info_log);
			throw std::runtime_error(info_log);
		}
	}
}


template <class Value>
void shader_program_t::set_uniform(const char* name, const Value& value) {
	use();
	auto location = glGetUniformLocation(id(), name);
	uniform_setter_t setter(location);
	setter.set_value(value);
}

template <class Value>
void shader_program_t::set_uniform_array(const char* name, const Value* value, const size_t size) {
	use();
	auto location = glGetUniformLocation(id(), name);
	uniform_setter_t setter(location);
	setter.set_value(value, size);
}


template <class texture_slot_iterator>
void shader_program_t::set_texture_array(const char* name, const texture_slot_iterator begin, const texture_slot_iterator end) {
	const auto size = std::distance(begin, end);
	use();
	auto location = glGetUniformLocation(id(), name);
	uniform_setter_t setter(location);
	if constexpr(std::is_integral_v<std::remove_pointer_t<texture_slot_iterator>>) {
		setter.set_value(begin, size);
	} else {
		std::vector<GLint> buffer(size);
		std::transform(
			begin,
			begin+size,
			buffer.begin(),
			[](const auto& slot){
				if constexpr(std::is_integral_v<std::remove_reference_t<decltype(slot)>>) {
					return slot;
				} else {
					return slot.id();
				}
			}
		);
		setter.set_value(buffer.data(), size);
	}
}

}

