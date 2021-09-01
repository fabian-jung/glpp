#include "glpp/core/object/shader.hpp"
#include <string>
#include <streambuf>
#include <iterator>

namespace glpp::core::object {

std::string add_line_numbers(const std::string& code) {
	std::string result;
	auto line_begin = 0;
	auto line_end = code.find('\n', line_begin);
	size_t line_number = 1;
	do {
		const auto line = std::to_string(line_number)+':'+code.substr(line_begin, line_end - line_begin);
		result += line+'\n';
		++line_number;
		line_begin = line_end+1;
	} while((line_end = code.find('\n', line_begin)) != code.npos);
	return result;
}

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
		throw std::runtime_error(add_line_numbers(code)+'\n'+infoLog);
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

shader_program_t::uniform_setter_t::uniform_setter_t(GLuint program, GLint location) :
	program(program),
	location(location)
{}


GLint shader_program_t::uniform_location(const char* name) const {
	return glGetUniformLocation(id(), name);
}

void shader_program_t::set_texture(const char* name, const texture_slot_t& slot) {
	set_texture(uniform_location(name), slot);
}

void shader_program_t::set_texture(GLint location, const texture_slot_t& slot) {
	set_uniform(location, slot.id());
}

template <>
void shader_program_t::uniform_setter_t::set_value(const bool& b) {
	GLint v = b;
	glProgramUniform1i(program, location, v);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec2& vec) {
	const glm::ivec2 values = vec;
	glProgramUniform2iv(program, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec3& vec) {
	const glm::ivec2 values = vec;
	glProgramUniform3iv(program, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::bvec4& vec) {
	const glm::ivec2 values = vec;
	glProgramUniform4iv(program, location, 1, glm::value_ptr(values));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float* begin, const size_t size) {
	glProgramUniform1fv(program, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const float& f) {
	glProgramUniform1f(program, location, f);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec2& vec) {
	glProgramUniform2fv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec2* begin, const size_t size) {
	glProgramUniform2fv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec3& vec) {
	glProgramUniform3fv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec3* begin, const size_t size) {
	glProgramUniform3fv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec4& vec) {
	glProgramUniform4fv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::vec4* begin, const size_t size) {
	glProgramUniform4fv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double& d) {
	glProgramUniform1d(program, location, d);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const double* begin, const size_t size) {
	glProgramUniform1dv(program, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec2& vec) {
	glProgramUniform2dv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec2* begin, const size_t size) {
	glProgramUniform2dv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec3& vec) {
	glProgramUniform3dv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec3* begin, const size_t size) {
	glProgramUniform3dv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec4& vec) {
	glProgramUniform4dv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dvec4* begin, const size_t size) {
	glProgramUniform4dv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint* begin, const size_t size) {
	glProgramUniform1iv(program, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLint& i) {
	glProgramUniform1i(program, location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec2& vec) {
	glProgramUniform2iv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec2* begin, const size_t size) {
	glProgramUniform2iv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec3& vec) {
	glProgramUniform3iv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec3* begin, const size_t size) {
	glProgramUniform3iv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec4& vec) {
	glProgramUniform4iv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::ivec4* begin, const size_t size) {
	glProgramUniform4iv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLuint& i) {
	glProgramUniform1ui(program, location, i);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const GLuint* begin, const size_t size) {
	glProgramUniform1uiv(program, location, size, begin);
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec2& vec) {
	glProgramUniform2uiv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec2* begin, const size_t size) {
	glProgramUniform2uiv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec3& vec) {
	glProgramUniform3uiv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec3* begin, const size_t size) {
	glProgramUniform3uiv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec4& vec) {
	glProgramUniform4uiv(program, location, 1, glm::value_ptr(vec));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::uvec4* begin, const size_t size) {
	glProgramUniform4uiv(program, location, size, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat2& mat) {
	glProgramUniformMatrix2fv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat2* begin, const size_t size) {
	glProgramUniformMatrix2fv(program, location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat3& mat) {
	glProgramUniformMatrix3fv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat3* begin, const size_t size) {
	glProgramUniformMatrix3fv(program, location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat4& mat) {
	glProgramUniformMatrix4fv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::mat4* begin, const size_t size) {
	glProgramUniformMatrix4fv(program, location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat2& mat) {
	glProgramUniformMatrix2dv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat2* begin, const size_t size) {
	glProgramUniformMatrix2dv(program, location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat3& mat) {
	glProgramUniformMatrix3dv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat3* begin, const size_t size) {
	glProgramUniformMatrix3dv(program, location, size, false, glm::value_ptr(*begin));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat4& mat) {
	glProgramUniformMatrix4dv(program, location, 1, false, glm::value_ptr(mat));
}

template <>
void shader_program_t::uniform_setter_t::set_value(const glm::dmat4* begin, const size_t size) {
	glProgramUniformMatrix4dv(program, location, size, false, glm::value_ptr(*begin));
}

void shader_program_t::use() const {
	glUseProgram(id());
}

void shader_program_t::attatch(const shader_t& shader) {
	glAttachShader(id(), shader.id());
}

void shader_program_t::link() {
	glLinkProgram(id());
	int success;
	char info_log[512];
	glGetProgramiv(id(), GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(id(), sizeof(info_log), NULL, info_log);
		throw std::runtime_error(info_log);
	}
}

void shader_program_t::destroy(GLuint id) {
	glDeleteProgram(id);
}

}
