#include <catch2/catch.hpp>
#include <glpp/asset.hpp>
#include <glpp/testing/context.hpp>

using namespace glpp::asset;
using namespace glpp::test;
using namespace glpp::core::object;

TEST_CASE("mesh render test", "[asset] [system] [xorg]") {
    constexpr auto width = 2;
    constexpr auto height = 2;

    context_t<offscreen_driver_t> context(width, height);
    const glm::vec3 clear_color { 0.0f };
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    const auto white_screen = context.swap_buffer();
    REQUIRE(white_screen == image_t<glm::vec3>(width, height, clear_color));

    constexpr auto default_material = 0;
   
    mesh_t mesh {
        mesh_t::model_t {
            {
                mesh_t::vertex_description_t{
                    {-1.0f, -1.0f, 0.0f}, // pos
                    {0.0f,   0.0f, 1.0f}, // normal
                    {0.0f,   1.0f}        // uv
                },
                mesh_t::vertex_description_t{
                    {1.0f,   1.0f, 0.0f}, // pos
                    {0.0f,   0.0f, 1.0f}, // normal
                    {1.0f,   0.0f}        // uv
                },
                mesh_t::vertex_description_t{
                    {-1.0f,  1.0f, 0.0f}, // pos
                    {0.0f,   0.0f, 1.0f}, // normal
                    {0.0f,   0.0f}        // uv
                },
                mesh_t::vertex_description_t{
                    {1.0f, -1.0f, 0.0f}, // pos
                    {0.0f,  0.0f, 1.0f}, // normal
                    {1.0f,  1.0f}        // uv
                },
            },
            {
                0, 1, 2,
                0, 3, 1
            }
            
        },
        glm::mat4(1.0f),
        default_material
    };

    material_t material;
    material.ambient = glm::vec3(1.0, 0.0, 0.0);
    material.emissive = glm::vec3(0.0, 1.0, 0.0);
    material.diffuse = glm::vec3(0.0, 0.0, 1.0);
    material.specular = glm::vec3(1.0, 0.0, 1.0);
    render::mesh_view_t view { mesh };


    SECTION("Normal Renderer") {
        render::mesh_renderer_t renderer { shading::normal_t{}, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});    
        renderer.render(view);
        const auto normal_screen = context.swap_buffer();
        REQUIRE( (normal_screen == image_t{ width, height, glm::vec3{.5, .5, 1} }).epsilon(0.05) );
    }
    
    SECTION("Depth Renderer") {
        render::mesh_renderer_t renderer { shading::depth_t{}, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});    
        renderer.render(view);
        auto normal_screen = context.swap_buffer();
        REQUIRE( (normal_screen == image_t{ width, height, glm::vec3{.5, .5, .5} }).epsilon(0.05) );

        glClear(GL_COLOR_BUFFER_BIT);
        renderer.update_model_matrix(glm::translate(glm::vec3(0,0,1)));
        renderer.render(view);
        normal_screen = context.swap_buffer();
        REQUIRE( (normal_screen == image_t{ width, height, glm::vec3{1, 1, 1} }).epsilon(0.05) );

        glClear(GL_COLOR_BUFFER_BIT);
        renderer.update_model_matrix(glm::translate(glm::vec3(0,0,-1)));
        renderer.render(view);
        normal_screen = context.swap_buffer();
        REQUIRE( (normal_screen == image_t{ width, height, glm::vec3{0, 0, 0} }).epsilon(0.05) );
    }

    SECTION("Flat Renderer diffuse") {  
        multi_atlas_t textures;
        render::mesh_renderer_t renderer { shading::flat_t{ textures }, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});
        renderer.render(view);
        auto flat_screen = context.swap_buffer();
        REQUIRE( (flat_screen == image_t{ width, height,  material.diffuse }).epsilon(0.05) );
    }

    SECTION("Flat Renderer ambient") {  
        multi_atlas_t textures;
        render::mesh_renderer_t renderer { shading::flat_t{ textures, shading::flat_shading_channel_t::ambient }, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});
        renderer.render(view);
        auto flat_screen = context.swap_buffer();
        REQUIRE( (flat_screen == image_t{ width, height,  material.ambient }).epsilon(0.05) );
    }

    SECTION("Flat Renderer emmisive") {  
        multi_atlas_t textures;
        render::mesh_renderer_t renderer { shading::flat_t{ textures, shading::flat_shading_channel_t::emission }, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});
        renderer.render(view);
        auto flat_screen = context.swap_buffer();
        REQUIRE( (flat_screen == image_t{ width, height,  material.emissive }).epsilon(0.05) );
    }
    
    SECTION("Flat Renderer ") {  
        multi_atlas_t textures;
        render::mesh_renderer_t renderer { shading::flat_t{ textures, shading::flat_shading_channel_t::specular }, material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});
        renderer.render(view);
        auto flat_screen = context.swap_buffer();
        REQUIRE( (flat_screen == image_t{ width, height,  material.specular }).epsilon(0.05) );
    }

    SECTION("Flat Texture") {
        multi_atlas_t texture_atlas;
        const image_t texture_base_image {
            2, 2,
            { 
                glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f)
            }
        };

        const auto tex_entry = 
            texture_atlas.insert(
                texture_base_image
            );
        
        const auto texture_slots = texture_atlas.bind_to_texture_slot();
        material.diffuse = glm::vec3(0.0, 0.0, 0.0);

        material.diffuse_textures.emplace_back(
            texture_stack_entry_t{
                tex_entry.key(),
                1.0f,
                op_t::addition
            }
        );
        render::mesh_renderer_t renderer { shading::flat_t{ texture_atlas } , material };
        renderer.update_model_matrix(glm::mat4{1.0f});
        renderer.update_projection(glm::mat4{1.0f});    
        renderer.update_texture_binding(texture_slots);
        renderer.render(view);
        auto flat_screen = context.swap_buffer();
        REQUIRE( (flat_screen == texture_base_image) );
    }


    SECTION("BlinnPhong Renderer") {

    }
}

TEST_CASE("scene renderer test", "[asset] [system] [xorg]") {
    constexpr auto width = 3;
    constexpr auto height = 3;
    context_t<offscreen_driver_t> context(width, height);
    const glm::vec3 clear_color { 1.0f };
    glClearColor(clear_color.r, clear_color.g, clear_color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    const auto white_screen = context.swap_buffer();
    REQUIRE(white_screen == image_t<glm::vec3>(width, height, clear_color));

    constexpr auto default_material = 0;
    scene_t scene;
    
    glpp::core::render::camera_t cam;
    cam.position.z = 1.0f;
    cam.fov = 45.0f;
    scene.cameras.emplace_back(cam);

    scene.materials.emplace_back(material_t{ "default_material" });
    
    scene.meshes.emplace_back(
        mesh_t{
            mesh_t::model_t {
                {
                    mesh_t::vertex_description_t{
                        {-1.0f, -1.0f, 0.0f}, // pos
                        {0.0f,   0.0f, 1.0f}, // normal
                        {0.0f,   0.0f}        // uv
                    },
                    mesh_t::vertex_description_t{
                        {1.0f,   1.0f, 0.0f}, // pos
                        {0.0f,   0.0f, 1.0f}, // normal
                        {1.0f,   1.0f}        // uv
                    },
                    mesh_t::vertex_description_t{
                        {-1.0f,  1.0f, 0.0f}, // pos
                        {0.0f,   0.0f, 1.0f}, // normal
                        {0.0f,   1.0f}        // uv
                    },
                    mesh_t::vertex_description_t{
                        {1.0f, -1.0f, 0.0f},  // pos
                        {0.0f,   0.0f, 1.0f}, // normal
                        {1.0f,   0.0f}        // uv
                    },
                },
                {
                    0, 1, 2,
                    0, 3, 1
                }
               
            },
            glm::mat4(1.0f),
            default_material
        }       
    );

    SECTION("normal renderer") {
        render::scene_view_t view { scene };
        render::scene_renderer_t renderer { shading::normal_t{}, scene };
        renderer.render(view, scene.cameras.front());
        const auto normal_screen = context.swap_buffer();
        REQUIRE( (normal_screen == image_t{ width, height, glm::vec3{.5, .5, 1} }).epsilon(0.05) );
    }


}