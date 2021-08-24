#include <catch2/catch.hpp>
#include <glpp/core/object/shader.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>
#include <sstream>
#include <cstring>

using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("shader_program_t calls constructor and destructor", "[core][unit]") {
    context.enable_throw();
    
    auto call_create = 0;
    auto call_destroy = 0;

    context.glCreateProgram = [&call_create]() -> GLuint {
        ++call_create;
        return 42;
    };
    context.glDeleteProgram = [&call_destroy](GLuint prog) {
        ++call_destroy;
        REQUIRE(prog == 42);
    };
    
    {
        shader_program_t shader_program;
        const auto shader_program_2 = std::move(shader_program);
    }

    REQUIRE(call_create == 1);
    REQUIRE(call_destroy == 1);
}

TEST_CASE("shader_program_t calls constructor and attatch shader", "[core][unit]") {
    context.enable_throw();

    auto call_attach = 0;
    auto call_link = 0;

    context.glCreateProgram = []() -> GLuint { return 42; };
    context.glDeleteProgram = [](auto...) {};
    context.glCreateShader = [](auto...) -> GLuint { return 43; };
    context.glDeleteShader = [](auto...) {};
    context.glShaderSource = [](auto...) {};
    context.glCompileShader = [](auto...) {};
    context.glGetShaderiv = [](GLuint, GLenum name, GLint* values) {
        if(name == GL_COMPILE_STATUS) {
            *values = GL_TRUE;
        }
    };
    context.glAttachShader = [&call_attach](GLuint prog, GLuint shader){
        ++call_attach;
        REQUIRE(prog == 42);
        REQUIRE(shader == 43);
    };
    context.glLinkProgram = [&call_link](GLuint prog){
        ++call_link;
        REQUIRE(prog == 42);
    };
    context.glGetProgramiv = [](GLuint, GLenum name, GLint* values){
        REQUIRE(name == GL_LINK_STATUS );
        *values = GL_TRUE;
    };
    shader_program_t shader_program {
        shader_t(
            shader_type_t::vertex,
            std::stringstream{"void main() {}"}
        )
    };

    REQUIRE(call_attach == 1);
    REQUIRE(call_link == 1);
}

TEST_CASE("shader_program_t use()", "[core][unit]") {
    context.enable_throw();

    auto call_use = 0;

    context.glCreateProgram = []() -> GLuint { return 42; };
    context.glDeleteProgram = [](auto...) {};
    context.glUseProgram = [&call_use](GLuint prog){
        ++call_use;
        REQUIRE(prog == 42);
    };
    const shader_program_t shader_program;
    shader_program.use();

    REQUIRE(call_use == 1);
}

TEST_CASE("shader_program_t set_uniform", "[core][unit]") {
    context.enable_throw();

    auto call_use = 0;

    context.glCreateProgram = []() -> GLuint { return 42; };
    context.glDeleteProgram = [](auto...) {};
    context.glGetUniformLocation = [](GLuint prog, const GLchar* name) -> GLint {
        REQUIRE(prog == 42);
        REQUIRE(std::strcmp(name, "test") == 0);
        return 1337;
    };
    context.glProgramUniform3fv = [&call_use](GLuint prog, GLint pos, GLsizei count, const GLfloat* values) {
        ++call_use;
        REQUIRE(prog == 42);
        REQUIRE(pos == 1337);
        REQUIRE(count == 1);
        REQUIRE(values[0] == 1.0f);
        REQUIRE(values[1] == 1.0f);
        REQUIRE(values[2] == 1.0f);
    };

    shader_program_t shader_program;
    shader_program.set_uniform("test", glm::vec3(1.0f));

    REQUIRE( call_use == 1);
}