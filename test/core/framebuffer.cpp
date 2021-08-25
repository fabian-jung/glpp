#include <catch2/catch.hpp>
#include <glpp/core/object/framebuffer.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>

using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("framebuffer construction and destruction", "[core][unit]") {
    context.enable_throw();

    auto call_create = 0;
    auto call_delete = 0;

    context.glCreateFramebuffers = [&call_create](GLsizei n, GLuint * buffers){
        ++call_create;
        REQUIRE(n == 1);
        *buffers = 42;
    };
    context.glDeleteFramebuffers = [&call_delete](GLsizei n, const GLuint * buffers){
        ++call_delete;
        REQUIRE(n == 1);
        REQUIRE(*buffers == 42);
    };

    {
        framebuffer_t buffer { 12, 15 };

        REQUIRE(buffer.width() == 12);
        REQUIRE(buffer.height() == 15);
        
        const framebuffer_t next = std::move(buffer);

        REQUIRE(next.width() == 12);
        REQUIRE(next.height() == 15);
    }

    REQUIRE(call_create == 1);
    REQUIRE(call_delete == 1);
}

TEST_CASE("framebuffer attatch texture", "[core][unit]") {
    context.enable_throw();

    auto calls_bind = 0;

    context.glCreateFramebuffers = [](GLsizei, GLuint * buffers){
        *buffers = 42;
    };
    context.glDeleteFramebuffers = [](auto...){};
    context.glNamedFramebufferTexture = [&calls_bind](GLuint framebuffer, GLenum attachment, GLuint texture, GLint level){
        ++calls_bind;
        REQUIRE(framebuffer == 42);
        REQUIRE(attachment == GL_COLOR_ATTACHMENT0);
        REQUIRE(texture == 43);
        REQUIRE(level == 0);
    };

    context.glCreateTextures = [](GLenum, GLsizei, GLuint* tex){
        *tex = 43;
    };
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [](auto...){};

    framebuffer_t buffer { 12, 15 };
    texture_t texture { 12, 15 };
    buffer.attach(texture, attachment_t::color);

    REQUIRE(calls_bind == 1);
}

TEST_CASE("framebuffer bind", "[core][unit]") {
    context.enable_throw();

    auto calls_bind = 0;

    context.glCreateFramebuffers = [](GLsizei, GLuint * buffers){
        *buffers = 42;
    };
    context.glDeleteFramebuffers = [](auto...){};
    context.glNamedFramebufferTexture = [](auto...){};
    context.glCheckNamedFramebufferStatus = [](auto...)-> GLuint { return GL_FRAMEBUFFER_COMPLETE;};
    context.glBindFramebuffer = [&calls_bind](GLenum target, GLuint id){
        ++calls_bind;
        REQUIRE(id == 42);
        REQUIRE(target == GL_READ_FRAMEBUFFER);
    };
    context.glCreateTextures = [](auto...){};
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [](auto...){};


    framebuffer_t framebuffer { 12, 15 };
    texture_t texture { 12, 15 };
    framebuffer.attach(texture, attachment_t::color);
    framebuffer.bind(framebuffer_target_t::read);

    REQUIRE( calls_bind == 1 );
}

TEST_CASE("framebuffer bind default", "[core][unit]") {
    context.enable_throw();

    struct param_t {
        GLenum target;
        GLuint id;
    };
    std::vector<param_t> bind_calls {
        {GL_READ_FRAMEBUFFER, 42},
        {GL_READ_FRAMEBUFFER, 0}
    };

    context.glCreateFramebuffers = [](GLsizei, GLuint * buffers){
        *buffers = 42;
    };
    context.glDeleteFramebuffers = [](auto...){};
    context.glNamedFramebufferTexture = [](auto...){};
    context.glCheckNamedFramebufferStatus = [](auto...)-> GLuint { return GL_FRAMEBUFFER_COMPLETE;};
    context.glBindFramebuffer = [&bind_calls](GLenum target, GLuint id){
        REQUIRE(bind_calls.size() > 0);
        const auto& param = bind_calls.front();
        REQUIRE(param.id == id);
        REQUIRE(param.target == target);
        bind_calls.erase(bind_calls.begin());
    };
    context.glCreateTextures = [](auto...){};
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [](auto...){};


    framebuffer_t framebuffer { 12, 15 };
    texture_t texture { 12, 15 };
    framebuffer.attach(texture, attachment_t::color);
    framebuffer.bind(framebuffer_target_t::read);
    framebuffer_t::bind_default_framebuffer(framebuffer_target_t::read);
    REQUIRE( bind_calls.size() == 0 );
}

TEST_CASE("framebuffer pixel_read", "[core][unit]") {
    context.enable_throw();

    auto call_setup_fb = 0;
    auto call_read_pixels = 0;
    context.glCreateFramebuffers = [](GLsizei, GLuint * buffers){ *buffers = 42; };
    context.glDeleteFramebuffers = [](auto...){};
    context.glNamedFramebufferTexture = [](auto...){};
    context.glNamedFramebufferReadBuffer = [&call_setup_fb](GLuint framebuffer, GLenum mode) {
        ++call_setup_fb;
        REQUIRE(framebuffer == 42);
        REQUIRE(mode == GL_COLOR_ATTACHMENT0);
    };
    context.glReadPixels = [&call_read_pixels](
        GLint x, GLint y, 
        GLsizei width, GLsizei height, 
        GLenum format, 
        GLenum type, 
        void* data
    ) {
        ++call_read_pixels;
        REQUIRE(x == 0);
        REQUIRE(y == 0);
        REQUIRE(width == 12);
        REQUIRE(height == 15);
        REQUIRE(format == GL_RGB);
        REQUIRE(type == GL_FLOAT);
        REQUIRE(data != nullptr);
    };
    const framebuffer_t framebuffer { 12, 15 };
    const auto result = framebuffer.pixel_read();
    REQUIRE(result.width() == 12);
    REQUIRE(result.height() == 15);
    REQUIRE(call_read_pixels == 1);
    REQUIRE(call_setup_fb == 1);
}

TEST_CASE("framebuffer_t check default framebuffer is not deleted", "[core][unit]") {
    context.enable_throw();

    auto call_read_pixels = 0;
    context.glGetIntegerv = [](	GLenum value, GLint* data) {
        REQUIRE(value == GL_SCISSOR_BOX);
        data[2] = 12;
        data[3] = 15;
    };
    context.glNamedFramebufferReadBuffer = [](GLuint framebuffer, GLenum mode) {
        REQUIRE(framebuffer == 0);
        REQUIRE(mode == GL_COLOR_ATTACHMENT0);
    };
    context.glReadPixels = [&call_read_pixels](
        GLint x, GLint y, 
        GLsizei width, GLsizei height, 
        GLenum format, 
        GLenum type, 
        void* data
    ) {
        ++call_read_pixels;
        REQUIRE(x == 0);
        REQUIRE(y == 0);
        REQUIRE(width == 12);
        REQUIRE(height == 15);
        REQUIRE(format == GL_RGB);
        REQUIRE(type == GL_FLOAT);
        REQUIRE(data != nullptr);
    };
    const framebuffer_t framebuffer = framebuffer_t::get_default_framebuffer();
    REQUIRE(framebuffer.width() == 12);
    REQUIRE(framebuffer.height() == 15);
    const auto result = framebuffer.pixel_read();
    REQUIRE(result.width() == 12);
    REQUIRE(result.height() == 15);
    REQUIRE(call_read_pixels == 1);
}

/* 
 * There are no render tests for framebuffer_t since they are the key component in
 * the test context and testing them against itself would be pointless.
 */