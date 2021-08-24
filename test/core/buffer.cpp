#include <catch2/catch.hpp>
#include <glpp/core/object/buffer.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>

using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("buffer construction and destruction", "[core][unit]") {
    
    context.enable_throw();
    auto create_called = 0;
    auto data_called = 0;
    auto destroy_called = 0;

    context.glCreateBuffers = [&create_called](GLsizei size, GLuint* id) {
        ++create_called;
        *id = 42;
        REQUIRE(size == 1);
    };
    context.glNamedBufferData = [&data_called](GLenum target, GLsizeiptr size, const void* data, GLenum usage) {
        REQUIRE(target == 42);
        REQUIRE(size == sizeof(float));
        REQUIRE(*reinterpret_cast<const float*>(data) == 13.37f);
        REQUIRE(usage == GL_STATIC_DRAW);
        
        ++data_called;
    };
    context.glDeleteBuffers = [&destroy_called](GLsizei size, const GLuint* id){
        ++destroy_called;
        REQUIRE(size == 1);
        REQUIRE(*id == 42);
    };

    std::vector vert { 13.37f };

    {
        buffer_t old_buffer { buffer_target_t::array_buffer, vert.data(), vert.size()*sizeof(float), buffer_usage_t::static_draw };
        buffer_t new_buffer = std::move(old_buffer);
    }
    REQUIRE(create_called == 1);
    REQUIRE(data_called == 1);
    REQUIRE(destroy_called == 1);

}

TEST_CASE("buffer bind", "[core][unit]") {
    
    context.enable_throw();
    auto bind_called = 0;


    context.glCreateBuffers = [](GLsizei, GLuint* id) {
        *id = 42;
    };
    context.glNamedBufferData = [](auto...) {};
    context.glDeleteBuffers = [](auto...){};
    context.glBindBuffer = [&bind_called](GLenum target, GLuint buffer){
        ++bind_called;
        REQUIRE(buffer == 42);
        REQUIRE(target == static_cast<GLenum>(buffer_target_t::array_buffer));
        REQUIRE(target == GL_ARRAY_BUFFER);
    };

    std::vector vert { 13.37f };
    buffer_t buffer{ buffer_target_t::array_buffer, vert.data(), vert.size()*sizeof(float), buffer_usage_t::static_draw };
    buffer.bind();

    REQUIRE(bind_called == 1);
}

TEST_CASE("buffer copy to and from gpu mem", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context(1,1);
    
    const std::vector vert { 13.37f };
    const buffer_t buffer{ buffer_target_t::array_buffer, vert.data(), vert.size()*sizeof(float), buffer_usage_t::static_draw };

    const auto gpu_vert = buffer.read();
    REQUIRE(vert == gpu_vert);
}