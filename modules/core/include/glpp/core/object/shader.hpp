#pragma once

#include "glpp/gl.hpp"
#include "glpp/core/object.hpp"
#include "glpp/core/object/texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <istream>

namespace glpp::core::object {

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

	void attatch(const shader_t& shader);
	void link();

	template <class... shader_t>
	explicit shader_program_t(const shader_t&... shader);

	GLint uniform_location(const GLchar* name) const;

	template <class Value>
	void set_uniform(const char* name, const Value& value);

	template <class Value>
	void set_uniform(GLint location, const Value& value);
	
	template <class Value>
	void set_uniform_array(const char* name, const Value* begin, const size_t size);

	template <class Value>
	void set_uniform_array(GLint location, const Value* begin, const size_t size);

	void set_texture(const char* name, const texture_slot_t& texture);
	void set_texture(GLint location, const texture_slot_t& texture);

	template <class texture_slot_iterator>
	void set_texture_array(const char* name, const texture_slot_iterator begin, const texture_slot_iterator end);

	template <class texture_slot_iterator>
	void set_texture_array(GLint location, const texture_slot_iterator begin, const texture_slot_iterator end);

	void use() const;

private:

	class uniform_setter_t {
	public:
		uniform_setter_t(GLuint program, GLint location);

		template <class T>
		void set_value(const T& value);

		template <class T>
		void set_value(const T* begin, const size_t size);

	private:
		GLuint program;
		GLint location;
	};

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
	}
}

template <class Value>
void shader_program_t::set_uniform(const char* name, const Value& value) {
	set_uniform(uniform_location(name), value);
}

template <class Value>
void shader_program_t::set_uniform(GLint location, const Value& value) {
	uniform_setter_t setter(id(), location);
	setter.set_value(value);
}

template <class Value>
void shader_program_t::set_uniform_array(const char* name, const Value* value, const size_t size) {
	set_uniform_array(uniform_location(name), value, size);
}

template <class Value>
void shader_program_t::set_uniform_array(GLint location, const Value* value, const size_t size) {
	uniform_setter_t setter(id(), location);
	setter.set_value(value, size);
}

template <class texture_slot_iterator>
void shader_program_t::set_texture_array(const char* name, const texture_slot_iterator begin, const texture_slot_iterator end) {
	set_texture_array(uniform_location(name), begin, end);
}

template <class texture_slot_iterator>
void shader_program_t::set_texture_array(GLint location, const texture_slot_iterator begin, const texture_slot_iterator end) {
	const auto size = std::distance(begin, end);
	use();
	uniform_setter_t setter(id(), location);
	if constexpr(std::is_integral_v<std::remove_pointer_t<texture_slot_iterator>>) {
		setter.set_value(begin, size);
	} else {
		std::vector<GLint> buffer(size);
		std::transform(
			begin,
			end,
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

