#include <catch2/catch.hpp>
#include <glpp/core/object/vertex_array.hpp>
#include <glpp/testing/context.hpp>
#include <glpp/gl/context.hpp>

using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("vertex_array constructor and destructor", "[core][unit]") {
    context.enable_throw();

    auto call_create = 0;
    auto call_destroy = 0;

    context.glCreateVertexArrays = [&call_create](GLsizei n, GLuint* buffers) {
        ++call_create;
        REQUIRE(n == 1);
        *buffers = 42;
    };
    context.glDeleteVertexArrays = [&call_destroy](GLsizei n, const GLuint* buffers ) {
        ++call_destroy;
        REQUIRE(n == 1);
        REQUIRE(*buffers == 42);

    };
    
    {
        vertex_array_t va;
        const vertex_array_t va2 = std::move(va);
    }

    REQUIRE(call_create == 1);
    REQUIRE(call_destroy == 1);
}

TEST_CASE("vertex_array bind", "[core][unit]") {
    context.enable_throw();

    auto call_bind = 0;

    context.glCreateVertexArrays = [](GLsizei n, GLuint* buffers) {
        REQUIRE(n == 1);
        *buffers = 42;
    };
    context.glDeleteVertexArrays = [](auto...) {};
    context.glBindVertexArray = [&call_bind](GLuint vao){
        ++call_bind;
        REQUIRE(vao == 42);
    };
    const vertex_array_t va;
    va.bind();
    REQUIRE(call_bind == 1);
}

TEST_CASE("vertex_array attatch", "[core][unit]") {
    context.enable_throw();

    struct {
        int buffer = 0;
        int enable = 0;
        int binding = 0;
        int format = 0;
    } call;

    context.glCreateVertexArrays = [](GLsizei n, GLuint* buffers) {
        REQUIRE(n == 1);
        *buffers = 42;
    };
    context.glVertexArrayVertexBuffer = [&call](GLuint vao, GLuint binding, GLuint buffer, GLintptr offset, GLintptr stride){
        ++call.buffer;
        REQUIRE(vao == 42);
        REQUIRE(binding == 0);
        REQUIRE(buffer == 43);
        REQUIRE(offset == 0);
        REQUIRE(stride == sizeof(float));
    };
    context.glEnableVertexArrayAttrib = [&call](GLuint vao, GLuint index){
        ++call.enable;
        REQUIRE(vao == 42);
        REQUIRE(index == 0);
    };
    context.glVertexArrayAttribBinding = [&call](GLuint vao, GLuint index, GLuint binding){
        ++call.binding;
        REQUIRE(vao == 42);
        REQUIRE(index == 0);
        REQUIRE(binding == 0);
    };
    context.glVertexArrayAttribFormat = [&call](
        GLuint vao,
        GLuint index,
        GLint elements,
        GLenum type,
        GLboolean normalized,
        GLintptr offset
    ){
        ++call.format;
        REQUIRE(vao == 42);
        REQUIRE(index == 0);
        REQUIRE(elements == 1);
        REQUIRE(type == GL_FLOAT);
        REQUIRE(normalized == GL_FALSE);
        REQUIRE(offset == 0);     
    };

    context.glDeleteVertexArrays = [](auto...) {};
    context.glCreateBuffers = [](GLsizei n, GLuint *buffers) {
        REQUIRE(n == 1);
        *buffers = 43;
    };
    context.glNamedBufferData = [](auto...) {};
    context.glDeleteBuffers = [](auto...){};

    const std::array vert { 13.37f };

    vertex_array_t va;
    buffer_t buffer { 
        buffer_target_t::array_buffer, 
        vert.data(),
        vert.size()*sizeof(float),
        buffer_usage_t::static_draw 
    };

    va.attach(buffer, 0);

    REQUIRE(call.enable == 1);
    REQUIRE(call.buffer == 1);
    REQUIRE(call.format == 1);
    REQUIRE(call.binding == 1);
}