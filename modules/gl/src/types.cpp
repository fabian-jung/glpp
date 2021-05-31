#include "definitions.hpp"

std::vector<type_definition_t> gl_types {
    {"void", "GLvoid"},
    {"std::uint32_t", "GLbitfield"},
    {"std::uint8_t", "GLboolean"},
    {"std::int8_t", "GLbyte"},
    {"char", "GLchar"},
    {"std::uint8_t", "GLubyte"},
    {"std::int16_t", "GLshort"},
    {"std::uint16_t", "GLushort"},
    {"std::int32_t", "GLint"},
    {"std::uint32_t", "GLuint"},
    {"std::int32_t", "GLsizei"},
    {"std::int32_t", "GLfixed"},
    {"std::int64_t", "GLlong"},
    {"std::int64_t", "GLint64"},
    {"std::uint64_t", "GLulong"},
    {"std::uint64_t", "GLuint64"},
    {"float", "GLfloat"},
    {"double", "GLdouble"},
    {"float", "GLclampf"},
    {"double", "GLclampd"},
    {"std::ptrdiff_t", "GLintptr"},
    {"std::size_t", "GLsizeiptr"},
    {"GLintptr", "GLsync"},
    {"std::uint32_t", "GLenum"},
    {"void (*)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)","GLDEBUGPROC"}
};