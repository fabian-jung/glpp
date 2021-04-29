#include <catch2/catch.hpp>

#include <glpp/core.hpp>
#include <filesystem>

TEST_CASE("Reading and Writing of images", "[image][unit][filesystem]") {
    using glpp::core::object::image_t;

    constexpr std::array extentions {
        "png",
        "jpg",
        "jpeg",
        "bmp",
        "tga"
    };

    const std::string seed = "xIk7gTNKCOX3G8GlwPR0-";

    for(const auto extention : extentions) {
        DYNAMIC_SECTION("testing " << extention) {
            const auto filename = seed+"test_image."+extention;

             if(std::filesystem::exists(filename)) {
                std::filesystem::remove(filename);
            }
            const image_t write {2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)
            }};
            write.write(filename.c_str());
            const image_t<glm::vec3>  load(filename.c_str());
            std::filesystem::remove(filename);
            REQUIRE((write == load).epsilon(0.05));
        }
    }
}