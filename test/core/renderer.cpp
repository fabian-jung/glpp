#include <catch2/catch.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/test/context.hpp>
#include <glpp/core/render.hpp>
#include <cstring>

#include <iostream>

using namespace glpp::core::render;
using namespace glpp::core::object;
using namespace glpp::gl;

TEST_CASE("renderer_t construction and destruction", "[core][unit]") {
    context.enable_throw();

    auto call_create = 0;
    auto call_delete = 0;

    context.glCreateProgram = [&call_create]() -> GLuint{
        ++call_create;
        return 42;
    };

    context.glDeleteProgram = [&call_delete](GLuint prog) {
        ++call_delete;
        REQUIRE(prog == 42);
    };

    {
        renderer_t renderer;
        const renderer_t renderer2 = std::move(renderer);
    }

    REQUIRE(call_create == 1);
    REQUIRE(call_delete == 1);
}

TEST_CASE("renderer_t set uniform", "[core][unit]") {
    context = mock_context_t{};
    context.enable_logging(std::cout);

    auto call_get_location = 0;
    auto call_uniform = 0;

    context.glCreateProgram = []() -> GLuint{
        return 42;
    };

    context.glDeleteProgram = [](GLuint) {};

    context.glGetUniformLocation = [&call_get_location](GLuint vao, const GLchar* name) -> GLint {
        ++call_get_location;
        REQUIRE(std::strcmp(name, "name") == 0);
        REQUIRE(vao == 42);
        return 43;
    };

    struct uniform_description_t {
        glm::vec3 name;
    };

    renderer_t<uniform_description_t> renderer;
    renderer.set_uniform_name(&uniform_description_t::name, "name");

    SECTION("uniform vec3") {
        context.glProgramUniform3fv = [&call_uniform](GLuint vao, GLint location, GLsizei count, const float* data) {
            ++call_uniform;
            REQUIRE(vao == 42);
            REQUIRE(location == 43);
            REQUIRE(count == 1);
            REQUIRE(data[0] == 1.0f);
            REQUIRE(data[1] == 1.0f);
            REQUIRE(data[2] == 1.0f);
        };

        renderer.set_uniform(&uniform_description_t::name, glm::vec3(1.0f));
        renderer.set_uniform(&uniform_description_t::name, glm::vec3(1.0f));
    }

    SECTION("uniform array vec3") {
        context.glProgramUniform3fv = [&call_uniform](GLuint vao, GLint location, GLsizei count, const float* data) {
            ++call_uniform;
            REQUIRE(vao == 42);
            REQUIRE(location == 43);
            REQUIRE(count == 3);
            REQUIRE(data[0] == 0.0f);
            REQUIRE(data[1] == 0.0f);
            REQUIRE(data[2] == 0.0f);
            REQUIRE(data[3] == 1.0f);
            REQUIRE(data[4] == 1.0f);
            REQUIRE(data[5] == 1.0f);
            REQUIRE(data[6] == 2.0f);
            REQUIRE(data[7] == 2.0f);
            REQUIRE(data[8] == 2.0f);
        };
        
        const std::array data {
            glm::vec3(0.0f),
            glm::vec3(1.0f),
            glm::vec3(2.0f)
        };

        renderer.set_uniform_array(&uniform_description_t::name, data.data(), data.size());
        renderer.set_uniform_array(&uniform_description_t::name, data.data(), data.size());
    }

    REQUIRE(call_get_location == 1);
    REQUIRE(call_uniform == 2);
}

TEST_CASE("renderer_t render screen-quad", "[core][render][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 2, 2 };

    struct vertex_description_t {
        glm::vec3 pos;
    };

    renderer_t renderer{
        shader_t(
			shader_type_t::vertex,
            R"(
                #version 450 core
                layout (location = 0) in vec3 pos;
                
                out vec2 tex;

                void main()
                {
                    gl_Position = vec4(pos, 1.0);
                }
            )"
        ),
        shader_t(
			shader_type_t::fragment,
            R"(
                #version 450 core
                out vec4 FragColor;


                void main()
                {
                    FragColor = vec4(0.0, 1.0, 0.0, 1.0);
                }
            )"
        )
    };

    const glpp::core::object::image_t<glm::vec3> reference {
        2, 2,
        {
            {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}
        }
    };

    SECTION("with non indexed view") {
        const model_t<vertex_description_t> model {
            {glm::vec3( 0, 0, 0 )},
            {glm::vec3( 1, 0, 0 )},
            {glm::vec3( 1, 1, 0 )},
            {glm::vec3( 0, 0, 0 )},
            {glm::vec3( 1, 1, 0 )},
            {glm::vec3( 0, 1, 0 )}
        };
        
        const view_t view { model, &vertex_description_t::pos };
        renderer.render(view);
    }

    SECTION("with indexed view") {
        const indexed_model_t<vertex_description_t, GLubyte> model {
            {
                {glm::vec3( 0, 0, 0 )},
                {glm::vec3( 1, 0, 0 )},
                {glm::vec3( 1, 1, 0 )},
                {glm::vec3( 0, 1, 0 )}
            } , {
                0, 1, 2, 0, 3, 2
            }
        };
        const view_t view { model, &vertex_description_t::pos };
        renderer.render(view);    
    }
    
    SECTION("with indexed view") {
        const indexed_model_t<vertex_description_t, GLushort> model {
            {
                {glm::vec3( 0, 0, 0 )},
                {glm::vec3( 1, 0, 0 )},
                {glm::vec3( 1, 1, 0 )},
                {glm::vec3( 0, 1, 0 )}
            } , {
                0, 1, 2, 0, 3, 2
            }
        };
        const view_t view { model, &vertex_description_t::pos };
        renderer.render(view);    
    }

    SECTION("with indexed view") {
        const indexed_model_t<vertex_description_t, GLuint> model {
            {
                {glm::vec3( 0, 0, 0 )},
                {glm::vec3( 1, 0, 0 )},
                {glm::vec3( 1, 1, 0 )},
                {glm::vec3( 0, 1, 0 )}
            } , {
                0, 1, 2, 0, 3, 2
            }
        };
        const view_t view { model, &vertex_description_t::pos };
        renderer.render(view);    
    }
    const auto result = context.swap_buffer();
    REQUIRE((result == reference));
}

TEST_CASE("renderer_t set_uniform render test", "[core][render][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 2, 2 };

    const glm::vec3 color { 0.5f, 0.25f, 0.125f };
    struct vertex_description_t {
        glm::vec3 pos;
    };

    struct uniform_description_t {
        glm::vec3 color;
    };

    renderer_t<uniform_description_t> renderer{
        shader_t(
			shader_type_t::vertex,
            R"(
                #version 450 core
                layout (location = 0) in vec3 pos;
                
                out vec2 tex;

                void main()
                {
                    gl_Position = vec4(pos, 1.0);
                }
            )"
        ),
        shader_t(
			shader_type_t::fragment,
            R"(
                #version 450 core
                out vec4 FragColor;

                uniform vec3 color;

                void main()
                {
                    FragColor = vec4(color, 1.0);
                }
            )"
        )
    };

    renderer.set_uniform_name(&uniform_description_t::color, "color");
    renderer.set_uniform(&uniform_description_t::color, color);

    const glpp::core::object::image_t<glm::vec3> reference {
        2, 2,
        {
            {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}, color
        }
    };

    const model_t<vertex_description_t> model {
        {glm::vec3( 0, 0, 0 )},
        {glm::vec3( 1, 0, 0 )},
        {glm::vec3( 1, 1, 0 )},
        {glm::vec3( 0, 0, 0 )},
        {glm::vec3( 1, 1, 0 )},
        {glm::vec3( 0, 1, 0 )}
    };
    
    const view_t view { model, &vertex_description_t::pos };
    renderer.render(view);
    
    const auto result = context.swap_buffer();
    REQUIRE((result == reference).epsilon(0.05f));
}