#include <catch2/catch.hpp>

#include <glpp/core.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>

using namespace glpp::core::object;

TEST_CASE("texture_atlas_t construction and destruction", "[core][unit]") {
    REQUIRE_NOTHROW(texture_atlas_t<texture_atlas::multi_policy_t>());   
}

TEST_CASE("texture_atlas_t alloc and free", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    REQUIRE(texture_atlas.keys().size() == 0);

    const auto key = texture_atlas.alloc();
    REQUIRE(texture_atlas.keys().size() == 1);
    REQUIRE(texture_atlas.keys().front() == key);

    texture_atlas.free(key);
    REQUIRE(texture_atlas.keys().size() == 0);
}

TEST_CASE("texture_atlas_t bad alloc", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    const auto key = texture_atlas.alloc();
    REQUIRE_THROWS(texture_atlas.alloc(key));
}

TEST_CASE("texture_atlas_t bad free", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;

    SECTION("Invalid free") {
        REQUIRE_THROWS(texture_atlas.free(0));
    }

    SECTION("Double free") {
        const auto key = texture_atlas.alloc();
        texture_atlas.free(key);
        REQUIRE_THROWS(texture_atlas.free(key));
    }
}

TEST_CASE("texture_atlas_t fetch", "[core][unit]") {
     glpp::gl::context = glpp::gl::mock_context_t{};

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    const auto key0 = texture_atlas.alloc();
    const auto key1 = texture_atlas.alloc();
    const auto key2 = texture_atlas.alloc();
    REQUIRE(texture_atlas.fetch("binding", key0, "uv") == "texture(binding[0], uv)");
    REQUIRE(texture_atlas.fetch("binding", key1, "uv") == "texture(binding[1], uv)");
    REQUIRE(texture_atlas.fetch("binding", key2, "uv") == "texture(binding[2], uv)");
}

TEST_CASE("texture_atlas static selection render test", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 2, 2 };

    const std::array references {
        image_t{
            2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), 
                glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 1.0)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 1.0), 
                glm::vec3(1.0, 1.0, 0.0), glm::vec3(1.0, 1.0, 1.0)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.5), 
                glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 0.5, 0.5)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(0.5, 0.0, 0.0), glm::vec3(0.5, 0.0, 1.0), 
                glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.5, 0.5, 0.5)
            }
        }
    };

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas_2;
    const std::array keys {
        texture_atlas.alloc(references[0]),
        texture_atlas.alloc(references[1]),
        texture_atlas_2.alloc(references[2]),
        texture_atlas_2.alloc(references[3])
    };
    const auto slot = texture_atlas.bind_to_texture_slot();
    const auto slot_2 = texture_atlas_2.bind_to_texture_slot();

    struct vertex_description_t {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    using namespace glpp::core::render;
    
    for(auto i = 0u; i < keys.size(); ++i) {
        const auto key = keys[i];
        const auto atlas = i<2 ? 0 : 1;
        DYNAMIC_SECTION("Render subimage " << key) {
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

                        )" +
                        texture_atlas.declaration("binding")+";"+                
                        R"(
                        void main()
                        {
                            FragColor = )" + texture_atlas.fetch("binding", key, "tex") +";"+
                    R"(
                        }
                    )"
                )
            };

            if(atlas == 0) {
                renderer.set_texture_atlas("binding", slot);
            } else {
                renderer.set_texture_atlas("binding", slot_2);
            }
            renderer.render(view);

            const auto rendered = context.swap_buffer();
            REQUIRE( (rendered == references[key]).epsilon(0.05f) );
        }
    }
}

TEST_CASE("texture_atlas dynamic selection render test", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 2, 2 };

    const std::array references {
        image_t{
            2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), 
                glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 1.0)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(1.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 1.0), 
                glm::vec3(1.0, 1.0, 0.0), glm::vec3(1.0, 1.0, 1.0)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.5), 
                glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 0.5, 0.5)
            }
        },
        image_t{
            2, 2, {
                glm::vec3(0.5, 0.0, 0.0), glm::vec3(0.5, 0.0, 1.0), 
                glm::vec3(0.5, 0.5, 0.0), glm::vec3(0.5, 0.5, 0.5)
            }
        }
    };

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    const std::array keys {
        texture_atlas.alloc(references[0]),
        texture_atlas.alloc(references[1]),
        texture_atlas.alloc(references[2]),
        texture_atlas.alloc(references[3])
    };
    const auto slot = texture_atlas.bind_to_texture_slot();

    struct vertex_description_t {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    using namespace glpp::core::render;
    
    for(std::int32_t tex_id = 0u; tex_id < static_cast<std::int32_t>(keys.size()); ++tex_id) {
        DYNAMIC_SECTION("Render subimage " << tex_id) {
            const model_t<vertex_description_t> model {
                {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                {glm::vec3(  1, -1, 0 ), glm::vec2( 1, 0 )},
                {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                {glm::vec3( -1,  1, 0 ), glm::vec2( 0, 1 )}
            };

            const view_t view { model };
            struct uniform_description_t {
                std::int32_t tex_id;
            };
            renderer_t<uniform_description_t> renderer{
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
                        uniform int tex_id;
                        )" +
                        texture_atlas.declaration("binding")+";"+                
                        R"(
                        void main()
                        {
                            FragColor = )" + texture_atlas.fetch("binding", "tex_id", "tex") +";"+
                    R"(
                        }
                    )"
                )
            };

            renderer.set_texture_atlas("binding", slot);
            renderer.set_uniform_name(&uniform_description_t::tex_id, "tex_id");
            renderer.set_uniform(&uniform_description_t::tex_id, tex_id);
            renderer.render(view);

            const auto rendered = context.swap_buffer();
            REQUIRE( (rendered == references[tex_id]).epsilon(0.05f) );
        }
    }
}