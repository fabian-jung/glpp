#include <catch2/catch.hpp>

#include <glpp/core.hpp>
#include <filesystem>

TEST_CASE("image_t open nonexistant file should throw", "[unit][filesystem]") {
    using glpp::core::object::image_t;
    const auto filename = "non_existant_test_image.png";
    REQUIRE(!std::filesystem::exists(filename));
    REQUIRE_THROWS(image_t<glm::vec3>(filename));
}

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

            REQUIRE(!std::filesystem::exists(filename));

            const image_t write {2, 2, {
                glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0, 0.0, 0.0),
                glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)
            }};
            write.write(filename.c_str());
            const image_t<glm::vec3> load(filename.c_str());
            std::filesystem::remove(filename);
            REQUIRE(!std::filesystem::exists(filename));
            REQUIRE((write == load).epsilon(0.05));
        }
    }
}