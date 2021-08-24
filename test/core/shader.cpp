#include <catch2/catch.hpp>
#include <glpp/core/object/shader.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>
#include <sstream>
#include <cstring>

using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("shader_t calls constructor and destructor", "[core][unit]") {
    glpp::gl::context.enable_throw();
    constexpr const char* code = 
    R"(
        #version 450
        void main() {
        }
    )";
    context.enable_throw();
    auto call_create = 0;
    auto call_delete = 0;
    auto call_source = 0;
    auto call_compile = 0;
    context.glCreateShader = [&call_create](GLenum type) -> GLuint {
        ++call_create;
        REQUIRE(type == GL_VERTEX_SHADER);
        return 42;
    };
    context.glDeleteShader = [&call_delete](GLuint shader) {
        ++call_delete;
        REQUIRE(shader == 42);
    };
    context.glShaderSource = [&call_source](GLuint shader, GLsizei count, const GLchar * const* string, const GLint* length) {
        REQUIRE(shader == 42);
        REQUIRE(count == 1);
        if(length == nullptr) {
            REQUIRE(std::strcmp(code, *string) == 0);
        } else {
            REQUIRE(*length == sizeof(code));
            REQUIRE(std::strncmp(code, *string, *length) == 0);
        }
       
        ++call_source;
    };
    context.glCompileShader = [&call_compile](GLuint shader) {
        ++call_compile;
        REQUIRE(shader == 42);
    };

    SECTION("Successful compilation") {
        context.glGetShaderiv = [](GLuint shader, GLenum name, GLint* values) {
            REQUIRE(shader == 42);
            REQUIRE(name == GL_COMPILE_STATUS);
            *values = GL_TRUE;
        };

        {
            shader_t shader { 
                shader_type_t::vertex,
                std::stringstream(
                    code
                )
            };
            const shader_t mov = std::move(shader);
        }

        REQUIRE(call_create == 1);
        REQUIRE(call_delete == 1);
    }

    SECTION("Unsuccessful compilation") {
        context.glGetShaderiv = [](GLuint shader, GLenum name, GLint* values) {
            REQUIRE(shader == 42);
            REQUIRE(name == GL_COMPILE_STATUS);
            *values = GL_FALSE;
        };
        const std::string error_msg = "Some compilation error.";

        auto call_log = 0;
        context.glGetShaderInfoLog = [&call_log, &error_msg](GLuint shader, GLsizei maxLength, GLsizei *length, GLchar* infoLog) {
            ++call_log;
            REQUIRE(shader == 42);
            REQUIRE(static_cast<size_t>(maxLength) > error_msg.size());
            if(length != nullptr) {
                *length = sizeof(error_msg);
            }
            std::strncpy(infoLog, error_msg.c_str(), error_msg.size());
        };

        REQUIRE_THROWS_WITH(
            shader_t(
                shader_type_t::vertex,
                std::stringstream(
                    code
                )
            ),
            error_msg
        );

        REQUIRE(call_log == 1);
        REQUIRE(call_create == 1);
        REQUIRE(call_delete == 1);
    }
}

TEST_CASE("shader_t construction and compilation", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 1, 1 };
    shader_t shader{
        shader_type_t::vertex,
        std::stringstream(
            R"(
            #version 450
            void main() {
            }
            )"
        )
    };
}


TEST_CASE("shader_t construction and compilation fail", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 1, 1 };
    REQUIRE_THROWS(
        shader_t{
            shader_type_t::vertex,
            std::stringstream(
                R"(
                #version 450
                void main() {
                    thisFunctionDoesNotExist();
                }
                )"
            )
        }
    );
}