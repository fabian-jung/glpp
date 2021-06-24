#include <catch2/catch.hpp>
#include <glpp/core/render/view.hpp>
#include <glpp/gl/context.hpp>
#include <cstring>

using namespace glpp::gl;
using namespace glpp::core::render;
using namespace glpp::core::object;

#include <iostream>

TEST_CASE("view_t construction and destruction", "[core][unit]") {
    context.enable_throw();
    context = mock_context_t{};

    bool instanced = false;

    auto call_create_vao = 0u;
    auto call_delete_vao = 0u;
    auto call_create_buffer = 0u;
    auto call_delete_buffer = 0u;
    auto call_set_buffer_data = 0u;
    auto call_enable_attrib = 0u;
    auto call_bind_attrib = 0u;
    auto call_bind_buffer = 0u;
    auto call_attrib_format = 0u;

    struct attribute_description_t {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    const model_t<attribute_description_t> model_data { 
        {{1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
        {{0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}
    };

    const std::vector indicies {{ 0, 1, 2 }};

    context.glCreateVertexArrays = [&call_create_vao](GLint n, GLuint* vao) {
        ++call_create_vao;
        REQUIRE(n == 1);
        *vao = 42;
    };

    context.glDeleteVertexArrays = [&call_delete_vao](GLint n, const GLuint* vao) {
        ++call_delete_vao;
        REQUIRE(n == 1);
        REQUIRE(*vao == 42);
    };

    context.glCreateBuffers = [&call_create_buffer](GLint n, GLuint* vao) {
        ++call_create_buffer;
        REQUIRE(n == 1);
        *vao = 43;
    };

    context.glDeleteBuffers = [&call_delete_buffer](GLint n, const GLuint* vao) {
        ++call_delete_buffer;
        REQUIRE(n == 1);
        REQUIRE(*vao == 43);
    };

    context.glNamedBufferData = 
        [
            &call_set_buffer_data,
            &model_data,
            &indicies,
            &instanced
        ](
            GLuint buffer,
            GLsizeiptr size,
            const void* data,
            GLenum usage
        ) {
            REQUIRE(buffer == 43);
            REQUIRE(usage == GL_STATIC_DRAW);
            if(instanced && call_set_buffer_data == 0) {
                // init instance buffer
                REQUIRE(size == indicies.size()*sizeof(GLuint));
                REQUIRE(std::memcmp(data, indicies.data(), size) == 0);
            } else {
                // init vertex buffer
                REQUIRE(size == model_data.size()*sizeof(attribute_description_t));
                REQUIRE(std::memcmp(data, model_data.data(), size) == 0);
            }
            ++call_set_buffer_data;
        };

    context.glVertexArrayVertexBuffer = [&call_bind_buffer](GLuint vao, GLuint binding_index, GLuint buffer, GLintptr offset, GLsizei stride) {
        ++call_bind_buffer;
        REQUIRE(vao == 42);
        REQUIRE(binding_index == 0);
        REQUIRE(buffer == 43);
        REQUIRE(offset == 0);
        REQUIRE(stride == sizeof(attribute_description_t));
    };

    context.glEnableVertexArrayAttrib = [&call_enable_attrib](GLuint vao, GLuint index) {
        REQUIRE(vao == 42);
        REQUIRE(index == call_enable_attrib);
        ++call_enable_attrib;
    };

    context.glVertexArrayAttribBinding = [&call_bind_attrib](GLuint vao, GLuint index, GLuint binding_index) {
        REQUIRE(vao == 42);
        REQUIRE(binding_index == 0);
        REQUIRE(index == call_bind_attrib);
        ++call_bind_attrib;
    };

    context.glVertexArrayAttribFormat = 
        [&call_attrib_format](
            GLuint vao,
            GLuint attribindex,
            GLint size,
            GLenum type,
            GLboolean normalized,
            GLuint relativeoffset
        ) {
            REQUIRE(vao == 42);
            REQUIRE(attribindex == call_attrib_format);
            if(call_attrib_format == 0) {
                REQUIRE(size == 3);
            } else {
                REQUIRE(size == 2);
            }
            REQUIRE(type == GL_FLOAT);
            REQUIRE(normalized == GL_FALSE);
            if(call_attrib_format == 0) {
                REQUIRE(relativeoffset == 0);
            } else {
                REQUIRE(relativeoffset == sizeof(glm::vec3));
            }
            ++call_attrib_format;
        };

    SECTION("Non instanced view") {
        {
            view_t view {
                model_data,
                &attribute_description_t::pos,
                &attribute_description_t::uv
            };

            const view_t view2 = std::move(view);
        }
        REQUIRE(call_create_buffer == 1);
        REQUIRE(call_delete_buffer == 1);
        REQUIRE(call_set_buffer_data == 1);

    }

    SECTION("Instanced view") {
        {
            instanced = true;
            view_t view {
                indexed_model_t { 
                    model_data,
                    indicies
                },
                &attribute_description_t::pos,
                &attribute_description_t::uv
            };

            const view_t view2 = std::move(view);
        }

        REQUIRE(call_create_buffer == 2);
        REQUIRE(call_delete_buffer == 2);
        REQUIRE(call_set_buffer_data == 2);
    }

    SECTION("Non instanced view without buffer specification") {
        {
            view_t view { model_data };

            const view_t view2 = std::move(view);
        }
        REQUIRE(call_create_buffer == 1);
        REQUIRE(call_delete_buffer == 1);
        REQUIRE(call_set_buffer_data == 1);

    }

    SECTION("Instanced view without buffer specification") {
        {
            instanced = true;
            view_t view {
                indexed_model_t { 
                    model_data,
                    indicies
                }
            };

            const view_t view2 = std::move(view);
        }

        REQUIRE(call_create_buffer == 2);
        REQUIRE(call_delete_buffer == 2);
        REQUIRE(call_set_buffer_data == 2);
    }

    REQUIRE(call_create_vao == 1);
    REQUIRE(call_delete_vao == 1);

    REQUIRE(call_enable_attrib == 2);
    REQUIRE(call_bind_attrib == 2);
    REQUIRE(call_bind_buffer == 1);
    REQUIRE(call_attrib_format == 2);
}