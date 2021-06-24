#include <catch2/catch.hpp>
#include <glpp/core/object/texture.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/test/context.hpp>
#include <glpp/core/render.hpp>

using namespace glpp::core::object;
using namespace glpp::core::render;
using namespace glpp::gl;

TEST_CASE("texture construction and destruction", "[core][unit]") {
    context.enable_throw();

    auto call_create = 0;
    auto call_delete = 0;

    context.glCreateTextures = [&call_create](GLenum target, GLsizei n, GLuint* textures){
        ++call_create;
        REQUIRE(target == GL_TEXTURE_2D);
        REQUIRE(n == 1);
        textures[0] = 42;
    };
    context.glDeleteTextures = [&call_delete](GLsizei n, const GLuint* textures){
        ++call_delete;
        REQUIRE(n == 1);
        REQUIRE(textures[0] == 42);
    };
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [](auto...){};

    {
        texture_t from{ 1, 1, image_format_t::rgb};
        texture_t to = std::move(from);
    }

    REQUIRE(call_create == 1);
    REQUIRE(call_delete == 1);
}

TEST_CASE("texture construction parameter and storage set", "[core][unit]") {
    context.enable_throw();

    auto calls_parameter = 0;
    auto calls_storage = 0;
    context.glCreateTextures = [](GLenum, GLsizei, GLuint* tex){
        *tex = 42;
    };
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [&calls_parameter](GLuint tex, GLenum name, GLint param){
        ++calls_parameter;
        REQUIRE(tex == 42);
        switch(name) {
            case GL_TEXTURE_WRAP_S:
            case GL_TEXTURE_WRAP_T:
                REQUIRE(param == static_cast<GLenum>(clamp_mode_t::clamp_to_edge));
                break;
            case GL_TEXTURE_MIN_FILTER:
            case GL_TEXTURE_MAG_FILTER:
                REQUIRE(param == static_cast<GLenum>(filter_mode_t::nearest));
                break;
            default:
                throw std::runtime_error("Invalid parameter set.");
        }
    };
    context.glTextureStorage2D = [&calls_storage](
        GLuint tex,
        GLsizei lod,
        GLenum internal,
        GLsizei width,
        GLsizei height
    ){
        ++calls_storage;
        REQUIRE(tex == 42);
        REQUIRE(lod == 1);
        REQUIRE(internal == static_cast<GLenum>(image_format_t::rgb));
        REQUIRE(width == 1337);
        REQUIRE(height == 1423);
    };

    texture_t texture{ 1337, 1423, image_format_t::rgb, clamp_mode_t::clamp_to_edge, filter_mode_t::nearest};

    REQUIRE(calls_parameter == 4);
    REQUIRE(calls_storage == 1);
}


TEST_CASE("texture construction from image_t", "[core][unit]") {
    context.enable_throw();

    auto calls_storage = 0;
    auto calls_subimg = 0;

    context.glCreateTextures = [](GLenum, GLsizei, GLuint* tex){
        *tex = 42;
    };
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [&calls_storage](
        GLuint tex,
        GLsizei lod,
        GLenum internal,
        GLsizei width,
        GLsizei height
    ){
        ++calls_storage;
        REQUIRE(tex == 42);
        REQUIRE(lod == 1);
        REQUIRE(internal == static_cast<GLenum>(image_format_t::rg_8));
        REQUIRE(width == 17);
        REQUIRE(height == 53);
    };
    context.glTextureSubImage2D = [&calls_subimg](
        GLuint tex, 
        GLint level, 
        GLint xoffset,
        GLint yoffset,
        GLsizei widht, 
        GLsizei height,
        GLenum format,
        GLenum type,
        const void* pixel
    ){
        ++calls_subimg;
        REQUIRE(tex == 42);
        REQUIRE(level == 0);
        REQUIRE(xoffset == 0);
        REQUIRE(yoffset == 0);
        REQUIRE(widht == 17);
        REQUIRE(height == 53);
        REQUIRE(format == static_cast<GLenum>(image_format_t::rg));
        REQUIRE(type == GL_UNSIGNED_BYTE);
        const auto begin = reinterpret_cast<const std::uint8_t*>(pixel);
        const auto end = begin +(widht*height*2);
        const auto all_pixels_have_the_right_value = std::all_of(
            begin,
            end,
            [](std::uint8_t v){ return v == 12; }
        );
        REQUIRE(all_pixels_have_the_right_value);
    };

    texture_t texture{ image_t(17,53, glm::vec<2, std::uint8_t>{ 12, 12}) };

    REQUIRE(calls_storage == 1);
    REQUIRE(calls_subimg == 1);
}

TEST_CASE("texture bind to slot", "[core][unit]") {
    context.enable_throw();

    auto calls_bind = 0;

    context.glCreateTextures = [](GLenum, GLsizei, GLuint* tex){
        *tex = 42;
    };
    context.glDeleteTextures = [](auto...){};
    context.glTextureParameteri = [](auto...){};
    context.glTextureStorage2D = [](auto...){};
    context.glTextureSubImage2D = [](auto...){};
    context.glGetIntegerv = [](GLenum name, GLint* data){ 
        if(name == GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS) {
            *data = 16;
        } else {
            throw std::runtime_error("Trying to call invalid function.");
        }
    };
    context.glBindTextureUnit = [&calls_bind](GLuint, GLuint texture){
        ++calls_bind;
        REQUIRE(texture == 42);
    };

    texture_t texture{ image_t<glm::vec3>(1,1) };
    {
        auto slot = texture.bind_to_texture_slot();
        const auto slot2 = std::move(slot);
    }
    REQUIRE(calls_bind == 1);
}

TEST_CASE("texture render test", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 3, 3 };

    const image_t reference( 3, 3, {
        glm::vec3(0.0, 1.0, 1.0), glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.1, 0.2, 0.3),
        glm::vec3(1.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 0.0),
        glm::vec3(1.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 1.0), glm::vec3(1.0, 1.0, 1.0)
    });

    const texture_t texture{ reference };
    const auto slot = texture.bind_to_texture_slot();

    struct vertex_description_t {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    const model_t<vertex_description_t> model {
        {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
        {glm::vec3(  1, -1, 0 ), glm::vec2( 1, 0 )},
        {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
        {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
        {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
        {glm::vec3( -1,  1, 0 ), glm::vec2( 0, 1 )}
    };

    const view_t view { model };

    renderer_t renderer{
        shader_t(
			shader_type_t::vertex,
            R"(
                #version 450 core
                layout (location = 0) in vec3 pos;
                layout (location = 1) in vec2 uv;
                
                out vec2 tex;

                void main()
                {
                    tex = uv;
                    gl_Position = vec4(pos, 1.0);
                }
            )"
        ),
        shader_t(
			shader_type_t::fragment,
            R"(
                #version 450 core
                in vec2 tex;
                out vec4 FragColor;

                uniform sampler2D sampler;

                void main()
                {
                    FragColor = texture(sampler, tex);
                }
            )"
        )
    };

    renderer.set_texture("sampler", slot);
    renderer.render(view);

    const auto rendered = context.swap_buffer();

    REQUIRE( (rendered == reference).epsilon(0.05f) );
}