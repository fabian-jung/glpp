#include "glpp/core/object/texture.hpp"
#include "glpp/core/object/texture_atlas.hpp"
#include "glpp/gl/constants.hpp"
#include "glpp/gl/functions.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>

#include <glpp/core.hpp>
#include <glpp/gl/context.hpp>
#include <glpp/testing/context.hpp>
#include <initializer_list>
#include <random>
#include <ranges>
#include <vector>

using namespace glpp::core::object;
using namespace glpp::core::render;

template <class TextureStorage>
class render_helper_t {
public:
    render_helper_t(const TextureStorage& storage) :
        storage(storage)
    {}

    std::string vertex_shader_code() const {
        return R"(
            #version 450 core
            layout (location = 0) in vec3 pos;
            layout (location = 1) in vec2 uv;
            
            out vec2 tex;

            void main()
            {
                tex = uv;
                gl_Position = vec4(pos, 1.0);
            }
        )";
    }

    std::string texture_declaration() const {
        if constexpr(std::is_same_v<TextureStorage, texture_t>) {
            return "uniform sampler2D binding";
        } else {
            return storage.declaration("binding");
        }
    }

    std::string texture_fetch(const auto entry) const {
        if constexpr(std::is_same_v<TextureStorage, texture_t>) {
            return "texture(binding, tex)";
        } else {
            return entry.fetch("binding", "tex");
        }
    }

    std::string fragment_shader_code(const auto entry) const {
        return R"(
            #version 450 core
            in vec2 tex;
            out vec4 FragColor;

            )" +
            texture_declaration()+";"+                
            R"(
            void main()
            {
                FragColor = )" + texture_fetch(entry) + ";"+
        R"(
            }
        )";
    }

    auto set_slot(renderer_t<>& renderer) {
        if constexpr(std::is_same_v<TextureStorage, texture_t>) {
            auto slot = storage.bind_to_texture_slot();
            renderer.set_texture("binding", slot);
            return slot;
        } else {
            auto slot = storage.bind_to_texture_slot();
            renderer.set_texture_atlas("binding", slot);
            return slot;
        }
    }

    void render(const auto entry) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        struct vertex_description_t {
            glm::vec3 pos;  
            glm::vec2 uv;
        };

        const model_t<vertex_description_t> model {
            {glm::vec3( -1, -1, 0 ), glm::vec2( -1, -1 )},
            {glm::vec3(  1, -1, 0 ), glm::vec2(  2, -1 )},
            {glm::vec3(  1,  1, 0 ), glm::vec2(  2,  2 )},
            {glm::vec3( -1, -1, 0 ), glm::vec2( -1, -1 )},
            {glm::vec3(  1,  1, 0 ), glm::vec2(  2,  2 )},
            {glm::vec3( -1,  1, 0 ), glm::vec2( -1,  2 )}
        };

        const view_t view { model };

        renderer_t<> renderer{
            shader_t(
                shader_type_t::vertex,
                vertex_shader_code()
            ),
            shader_t(
                shader_type_t::fragment,
                fragment_shader_code(entry)
            )
        };

        // bind texture accessor
        const auto slot = set_slot(renderer);
        renderer.render(view);
    }



private:       
    const TextureStorage& storage;
};

const std::array reference_textures {
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
    },

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
    },
    image_t{
        2, 2, {
            glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.5), 
            glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.0, 0.5, 0.5)
        }
    },
};

template<class TextureAtlas>
void test(const image_format_t image_format, const clamp_mode_t clamp_mode, const filter_mode_t filter_mode, const mipmap_mode_t mipmap_mode, const swizzle_mask_t swizzle_mask) {
    std::array<image_t<glm::vec3>, reference_textures.size()> render_references;
    glpp::test::context_t<glpp::test::offscreen_driver_t> context { 36, 36 };

    std::transform(
        reference_textures.begin(),
        reference_textures.end(),
        render_references.begin(),
        [&](const image_t<glm::vec3>& reference){
            texture_t texture { reference, image_format, clamp_mode, filter_mode, mipmap_mode, swizzle_mask };
            render_helper_t render_helper { texture };
            render_helper.render(0);
            return context.swap_buffer();
        }
    );

    auto atlas = [&](){
        if constexpr(std::is_same_v<TextureAtlas, grid_atlas_t>) {
            return grid_atlas_t(3, 3, 2, 2, image_format, clamp_mode, filter_mode, mipmap_mode, swizzle_mask);
        } else {
            return TextureAtlas(image_format, clamp_mode, filter_mode, mipmap_mode, swizzle_mask);
        }
    }();

    std::vector<typename TextureAtlas::entry_t> entries;
    std::transform(
        reference_textures.begin(),
        reference_textures.end(),
        std::back_inserter(entries),
        [&](const image_t<glm::vec3>& reference){
            return atlas.insert(reference);
        }
    );

    for(size_t i = 0; i < reference_textures.size(); ++i) {
        const auto entry = entries[i];
        render_helper_t render_helper { atlas };
        render_helper.render(entry);
        const auto rendered_image = context.swap_buffer();
        REQUIRE((rendered_image == render_references[i]).epsilon(0.05f));
    }
}

const std::array image_formats {
    image_format_t::rgb_8,
    image_format_t::rgb_32f,
    image_format_t::rgba_16i        
};

const std::array clamp_modes {
    clamp_mode_t::clamp_to_border,
    clamp_mode_t::clamp_to_edge,
    clamp_mode_t::mirrored_repeat,
    clamp_mode_t::repeat        
};
const std::array filter_modes {
    filter_mode_t::linear,
    filter_mode_t::nearest
};

const std::array mipmap_modes {
    mipmap_mode_t::none
    // mipmap_mode_t::linear, // not supported
    // mipmap_mode_t::nearest, // not supported
};

const std::array swizzle_masks {
    swizzle_mask_t{texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::one },
    swizzle_mask_t{texture_channel_t::blue, texture_channel_t::green, texture_channel_t::red, texture_channel_t::one },
    swizzle_mask_t{texture_channel_t::zero, texture_channel_t::zero, texture_channel_t::zero, texture_channel_t::red },
};

TEST_CASE("texture_atlas render test using multi policy", "[core][system]") {
    for(const auto image_format : image_formats) {
        for(const auto clamp_mode : clamp_modes) {
            for(const auto filter_mode : filter_modes) {
                for(const auto mipmap_mode : mipmap_modes) {
                    for(const auto swizzle_mask : swizzle_masks) {
                        test<multi_atlas_t>(image_format, clamp_mode, filter_mode, mipmap_mode, swizzle_mask);
                    }
                }
            }
        }
    }
}

TEST_CASE("texture_atlas render test using grid policy", "[core][system]") {
    for(const auto image_format : image_formats) {
        for(const auto clamp_mode : clamp_modes) {
            for(const auto filter_mode : filter_modes) {
                for(const auto mipmap_mode : mipmap_modes) {
                    for(const auto swizzle_mask : swizzle_masks) {
                        test<grid_atlas_t>(image_format, clamp_mode, filter_mode, mipmap_mode, swizzle_mask);
                    }
                }
            }
        }
    }
}