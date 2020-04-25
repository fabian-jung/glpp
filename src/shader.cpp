#include "glpp/object/shader.hpp"
#include <string>
#include <streambuf>
#include <iterator>

namespace glpp::object {

void shader_t::init(const std::string& code) {
	if(code.size() == 0) throw std::runtime_error("Trying to compile shader with no code.");
	auto* c_str = code.c_str();
	call(glShaderSource, id(), 1, &c_str, nullptr);
	call(glCompileShader, id());
	int  success;
	char infoLog[512];
	call(glGetShaderiv,id(), GL_COMPILE_STATUS, &success);
	if(!success)
	{
		call(glGetShaderInfoLog,id(), sizeof(infoLog), nullptr, infoLog);
		throw std::runtime_error(infoLog);
	}
}
shader_t::shader_t(shader_type_t type, const std::string& code) :
	object_t(
		call(glCreateShader, static_cast<GLenum>(type)),
		destroy
	)
{
	init(code);
}

shader_t::shader_t(shader_type_t type, std::istream& code) :
	object_t(
		call(glCreateShader, static_cast<GLenum>(type)),
		destroy
	)
{
	init(
		std::string(
			(std::istreambuf_iterator<char>(code)),
			(std::istreambuf_iterator<char>())
		)
	);
}

shader_t::shader_t(shader_type_t type, std::istream&& code) :
	object_t(
		call(glCreateShader, static_cast<GLenum>(type)),
		destroy
	)
{
	init(
		std::string(
			std::istreambuf_iterator<char>(code),
			std::istreambuf_iterator<char>()
		)
	);
}

void shader_t::destroy(GLuint id) {
	call(glDeleteShader, id);
}

shader_program_t::uniform_setter_t::uniform_setter_t(GLint location) :
	location(location)
{}

void shader_program_t::set_texture(const char* name, const texture_slot_t& slot) {
	set_uniform(name, slot.id());
}

template <>
void shader_program_t::uniform_setter_t::set_value(const bool& b) {
	GLint v = b;
	glpp::call(glUniform1i, location, v);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec2& vec) {
	const glm::ivec2 values = vec;
	glpp::call(glUniform2iv, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec3& vec) {
	const glm::ivec2 values = vec;
	glpp::call(glUniform3iv, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec4& vec) {
	const glm::ivec2 values = vec;
	glpp::call(glUniform4iv, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float* begin, const size_t size) {
	glpp::call(glUniform1fv, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float& f) {
	glpp::call(glUniform1f, location, f);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec2& vec) {
	glpp::call(glUniform2fv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec3& vec) {
	glpp::call(glUniform3fv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec4& vec) {
	glpp::call(glUniform4fv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double& d) {
	glpp::call(glUniform1d, location, d);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double* begin, const size_t size) {
	glpp::call(glUniform1dv, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec2& vec) {
	glpp::call(glUniform2dv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec3& vec) {
	glpp::call(glUniform3dv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec4& vec) {
	glpp::call(glUniform4dv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint* begin, const size_t size) {
	glpp::call(glUniform1iv, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint& i) {
	glpp::call(glUniform1i, location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec2& vec) {
	glpp::call(glUniform2iv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec3& vec) {
	glpp::call(glUniform3iv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec4& vec) {
	glpp::call(glUniform4iv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLuint& i) {
	glpp::call(glUniform1ui, location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec2& vec) {
	glpp::call(glUniform2uiv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec3& vec) {
	glpp::call(glUniform3uiv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec4& vec) {
	glpp::call(glUniform4uiv, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat2& mat) {
	glpp::call(glUniformMatrix2fv, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat3& mat) {
	glpp::call(glUniformMatrix3fv, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat4& mat) {
	glpp::call(glUniformMatrix4fv, location, 1, false, glm::value_ptr(mat));
}

void shader_program_t::use() {
	call(glUseProgram,id());
}

void shader_program_t::attatch(const shader_t& shader) {
	glpp::call(glAttachShader,id(), shader.id());
}

void shader_program_t::link() {
	glpp::call(glLinkProgram,id());
}

void shader_program_t::destroy(GLuint id) {
	call(glDeleteProgram, id);
}

}
