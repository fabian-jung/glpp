#include <catch2/catch_all.hpp>

#include <glpp/core.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>

#include <tuple>

using namespace glpp::core::object;

template <class Functor>
void for_each_allocation_policy(Functor f) {
    f(texture_atlas::multi_policy_t{});
    f(texture_atlas::grid_policy_t{3, 3, 2, 2});
}
#include <iostream>

TEST_CASE("texture_atlas_t construction and destruction", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};
    for_each_allocation_policy(
        [](auto policy){
            REQUIRE_NOTHROW(texture_atlas_t(std::move(policy)));   
        }
    );
}

TEST_CASE("texture_atlas_t alloc and free", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    for_each_allocation_policy(
        [](auto policy){
            texture_atlas_t texture_atlas(std::move(policy));
            REQUIRE(texture_atlas.keys().size() == 0);
            REQUIRE(texture_atlas.keys().empty());

            const auto entry = texture_atlas.insert();
            const auto key = entry.key();

            REQUIRE(texture_atlas.size() == 1);
            REQUIRE(!texture_atlas.empty());
            REQUIRE(texture_atlas.keys().size() == 1);
            REQUIRE(texture_atlas.keys().front() == key);

            DYNAMIC_SECTION("Erase by  entry") {
                texture_atlas.erase(entry);
                REQUIRE(texture_atlas.empty());
                REQUIRE(texture_atlas.size() == 0);
                REQUIRE(texture_atlas.keys().size() == 0);
            }

            DYNAMIC_SECTION("Erase by  key") {
                texture_atlas.erase(key);
                REQUIRE(texture_atlas.empty());
                REQUIRE(texture_atlas.size() == 0);
                REQUIRE(texture_atlas.keys().size() == 0);
            }
        }
    );
}

TEST_CASE("texture_atlas_t bad alloc", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    for_each_allocation_policy(
        [](auto policy){
            texture_atlas_t texture_atlas(std::move(policy));

            const auto entry = texture_atlas.insert();
            REQUIRE_THROWS(texture_atlas.insert(entry.key()));
        }
    );
}

TEST_CASE("texture_atlas_t bad free", "[core][unit]") {
    glpp::gl::context = glpp::gl::mock_context_t{};

    for_each_allocation_policy(
        [](auto policy){
            texture_atlas_t texture_atlas(std::move(policy));

            DYNAMIC_SECTION("Invalid free") {
                REQUIRE_THROWS(texture_atlas.erase(typename decltype(texture_atlas)::key_t{}));
            }

            DYNAMIC_SECTION("Double free") {
                const auto entry = texture_atlas.insert();
                texture_atlas.erase(entry);
                REQUIRE_THROWS(texture_atlas.erase(entry));
            }
        }
    );
}

TEST_CASE("texture_atlas_t fetch", "[core][unit]") {
     glpp::gl::context = glpp::gl::mock_context_t{};

    texture_atlas_t<texture_atlas::multi_policy_t> texture_atlas;
    std::array entries = {
       texture_atlas.insert(),
       texture_atlas.insert(),
       texture_atlas.insert()
    };
    REQUIRE(entries[0].fetch("binding", "uv") == "texture(binding[0], uv)");
    REQUIRE(entries[1].fetch("binding", "uv") == "texture(binding[1], uv)");
    REQUIRE(entries[2].fetch("binding", "uv") == "texture(binding[2], uv)");
}

struct null_t {};
template <class UniformDescription = null_t>
class texture_atlas_renderer_t : public glpp::core::render::renderer_t<UniformDescription> {
public:
    template <class TextureAtlas>
    texture_atlas_renderer_t(const TextureAtlas& ta) :
        glpp::core::render::renderer_t<UniformDescription>{
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
                    ta.declaration("binding")+";"+                
                    R"(
                    void main()
                    {
                        FragColor = )" + ta.dynamic_fetch("binding", "tex_id", "tex") +";"+
                R"(
                    }
                )"
            )
        }
    {}
};

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

    std::array texture_atlases {
        texture_atlas_t<texture_atlas::multi_policy_t>{},
        texture_atlas_t<texture_atlas::multi_policy_t>{}
    };
    
    const std::array entries {
        texture_atlases[0].insert(references[0]),
        texture_atlases[0].insert(references[1]),
        texture_atlases[1].insert(references[2]),
        texture_atlases[1].insert(references[3])
    };

    const std::array slots {
        texture_atlases[0].bind_to_texture_slot(),
        texture_atlases[1].bind_to_texture_slot()
    };

    struct vertex_description_t {
        glm::vec3 pos;
        glm::vec2 uv;
    };

    using namespace glpp::core::render;
    
    for(auto i = 0u; i < entries.size(); ++i) {
        const auto entry = entries[i];
        const auto atlas = i<2 ? 0 : 1;
        DYNAMIC_SECTION("Render subimage " << entry.key()) {
            const model_t<vertex_description_t> model {
                {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                {glm::vec3(  1, -1, 0 ), glm::vec2( 1, 0 )},
                {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                {glm::vec3( -1,  1, 0 ), glm::vec2( 0, 1 )}
            };

            const view_t view { model };

            glpp::core::render::renderer_t renderer {
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
                        texture_atlases[atlas].declaration("binding")+";"+                
                        R"(
                        void main()
                        {
                            FragColor = )" + texture_atlases[atlas].fetch("binding", entry.key(), "tex") +";"+
                    R"(
                        }
                    )"
                )
            };

            renderer.set_texture_atlas("binding", slots[atlas]);
            renderer.render(view);

            const auto rendered = context.swap_buffer();
            std::cout << "entry = " << i << " atlas = " << atlas << std::endl;
            REQUIRE( (rendered == references[entry.key()]).epsilon(0.05f) );
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

    for_each_allocation_policy(
        [&](auto policy){
            texture_atlas_t texture_atlas(std::move(policy));
            
            const std::array entries {
                texture_atlas.insert(references[0]),
                texture_atlas.insert(references[1]),
                texture_atlas.insert(references[2]),
                texture_atlas.insert(references[3])
            };
            const auto slot = texture_atlas.bind_to_texture_slot();

            struct vertex_description_t {
                glm::vec3 pos;
                glm::vec2 uv;
            };

            using namespace glpp::core::render;
            
            for(std::int32_t tex_id = 0u; tex_id < static_cast<std::int32_t>(entries.size()); ++tex_id) {
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

                    texture_atlas_renderer_t<uniform_description_t> renderer(texture_atlas);

                    renderer.set_texture_atlas("binding", slot);
                    renderer.set_uniform_name(&uniform_description_t::tex_id, "tex_id");
                    renderer.set_uniform(&uniform_description_t::tex_id, tex_id);
                    renderer.render(view);

                    const auto rendered = context.swap_buffer();
                    REQUIRE( (rendered == references[tex_id]).epsilon(0.05f) );
                }
            }
        }
    );
}

TEST_CASE("texture_atlas clamping modes render test", "[core][system][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 2, 2 };

    const image_t reference {
        2, 2, {
            glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 1.0), 
            glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 1.0)
        }
    };

    for_each_allocation_policy(
        [&](auto policy){
            texture_atlas_t texture_atlas(std::move(policy));
            
            const auto entry = texture_atlas.insert(reference);
            const auto slot = texture_atlas.bind_to_texture_slot();

            struct vertex_description_t {
                glm::vec3 pos;
                glm::vec2 uv;
            };

            using namespace glpp::core::render;
            
            DYNAMIC_SECTION("Render subimage 0") {
                const model_t<vertex_description_t> model {
                    {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                    {glm::vec3(  1, -1, 0 ), glm::vec2( 1, 0 )},
                    {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                    {glm::vec3( -1, -1, 0 ), glm::vec2( 0, 0 )},
                    {glm::vec3(  1,  1, 0 ), glm::vec2( 1, 1 )},
                    {glm::vec3( -1,  1, 0 ), glm::vec2( 0, 1 )}
                };

                const view_t view { model };
                using key_t = typename decltype(policy)::key_t;
                struct uniform_description_t {
                    key_t tex_id;
                };
                texture_atlas_renderer_t<uniform_description_t> renderer(texture_atlas);
                

                renderer.set_texture_atlas("binding", slot);
                renderer.set_uniform_name(&uniform_description_t::tex_id, "tex_id");
                renderer.set_uniform(&uniform_description_t::tex_id, entry.key());
                renderer.render(view);

                const auto rendered = context.swap_buffer();
                REQUIRE( (rendered == reference).epsilon(0.05f) );
            }
        }
    );
}