#include "glpp/core/object/shader.hpp"
#include <string>
#include <streambuf>
#include <iterator>

namespace glpp::core::object {

void shader_t::init(const std::string& code) {
	if(code.size() == 0) throw std::runtime_error("Trying to compile shader with no code.");
	auto* c_str = code.c_str();
	glShaderSource(id(), 1, &c_str, nullptr);
	glCompileShader(id());
	int  success;
	char infoLog[512];
	glGetShaderiv(id(), GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(id(), sizeof(infoLog), nullptr, infoLog);
		throw std::runtime_error(infoLog);
	}
}
shader_t::shader_t(shader_type_t type, const std::string& code) :
	object_t(
		glCreateShader(static_cast<GLenum>(type)),
		destroy
	)
{
	init(code);
}

shader_t::shader_t(shader_type_t type, std::istream& code) :
	object_t(
		glCreateShader(static_cast<GLenum>(type)),
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
		glCreateShader(static_cast<GLenum>(type)),
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
	glDeleteShader(id);
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
	glUniform1i(location, v);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec2& vec) {
	const glm::ivec2 values = vec;
	glUniform2iv(location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec3& vec) {
	const glm::ivec2 values = vec;
	glUniform3iv(location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec4& vec) {
	const glm::ivec2 values = vec;
	glUniform4iv(location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float* begin, const size_t size) {
	glUniform1fv(location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float& f) {
	glUniform1f(location, f);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec2& vec) {
	glUniform2fv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec2* begin, const size_t size) {
	glUniform2fv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec3& vec) {
	glUniform3fv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec3* begin, const size_t size) {
	glUniform3fv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec4& vec) {
	glUniform4fv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec4* begin, const size_t size) {
	glUniform4fv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double& d) {
	glUniform1d(location, d);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double* begin, const size_t size) {
	glUniform1dv(location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec2& vec) {
	glUniform2dv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec2* begin, const size_t size) {
	glUniform2dv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec3& vec) {
	glUniform3dv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec3* begin, const size_t size) {
	glUniform3dv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec4& vec) {
	glUniform4dv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec4* begin, const size_t size) {
	glUniform4dv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint* begin, const size_t size) {
	glUniform1iv(location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint& i) {
	glUniform1i(location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec2& vec) {
	glUniform2iv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec2* begin, const size_t size) {
	glUniform2iv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec3& vec) {
	glUniform3iv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec3* begin, const size_t size) {
	glUniform3iv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec4& vec) {
	glUniform4iv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec4* begin, const size_t size) {
	glUniform4iv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLuint& i) {
	glUniform1ui(location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLuint* begin, const size_t size) {
	glUniform1uiv(location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec2& vec) {
	glUniform2uiv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec2* begin, const size_t size) {
	glUniform2uiv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec3& vec) {
	glUniform3uiv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec3* begin, const size_t size) {
	glUniform3uiv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec4& vec) {
	glUniform4uiv(location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec4* begin, const size_t size) {
	glUniform4uiv(location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat2& mat) {
	glUniformMatrix2fv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat2* begin, const size_t size) {
	glUniformMatrix2fv(location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat3& mat) {
	glUniformMatrix3fv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat3* begin, const size_t size) {
	glUniformMatrix3fv(location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat4& mat) {
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat4* begin, const size_t size) {
	glUniformMatrix4fv(location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat2& mat) {
	glUniformMatrix2dv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat2* begin, const size_t size) {
	glUniformMatrix2dv(location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat3& mat) {
	glUniformMatrix3dv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat3* begin, const size_t size) {
	glUniformMatrix3dv(location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat4& mat) {
	glUniformMatrix4dv(location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat4* begin, const size_t size) {
	glUniformMatrix4dv(location, size, false, glm::value_ptr(*begin));
}

void shader_program_t::use() {
	glUseProgram(id());
}

void shader_program_t::attatch(const shader_t& shader) {
	glAttachShader(id(), shader.id());
}

void shader_program_t::link() {
	glLinkProgram(id());
}

void shader_program_t::destroy(GLuint id) {
	glDeleteProgram(id);
}

}
