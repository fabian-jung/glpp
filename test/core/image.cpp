#include <catch2/catch.hpp>

#include <glpp/core.hpp>

TEST_CASE("image attribute check for vec3", "[core][unit]") {
    const glm::vec3 test_color {0.9, 0.8, 0.1};
    const glpp::core::object::image_t<glm::vec3> image(1, 1 , test_color);
    
    REQUIRE(image.width() == 1);
    REQUIRE(image.height() == 1);
    REQUIRE(*image.data() == test_color);
    REQUIRE(image.get(0, 0) == test_color);
    REQUIRE(image.at(0, 0) == test_color);
    REQUIRE_THROWS(image.at(1, 0));
    REQUIRE(image.channels() == 3);
    REQUIRE(image.format() == glpp::core::object::image_format_t::rgb);
    REQUIRE(image.type() == GL_FLOAT);
}

TEST_CASE("image attribute check for ubyte3", "[core][unit]") {
    using ubyte3 = glm::vec<3, unsigned char>;
    const ubyte3 test_color {123, 45, 67};
    glpp::core::object::image_t<ubyte3> image(1, 1, test_color);

    REQUIRE(image.width() == 1);
    REQUIRE(image.height() == 1);
    REQUIRE(*image.data() == test_color);
    REQUIRE(image.get(0, 0) == test_color);
    REQUIRE(image.at(0, 0) == test_color);
    REQUIRE_THROWS(image.at(0, 1));
    REQUIRE(image.channels() == 3);
    REQUIRE(image.format() == glpp::core::object::image_format_t::rgb);
    REQUIRE(image.type() == GL_UNSIGNED_BYTE);
}


TEST_CASE("image attribute check for ubyte", "[core][unit]") {
    using ubyte = unsigned char;
    glpp::core::object::image_t<ubyte> image(
        2, 2,
        { 
            42, 43,
            44, 45
        }
    );

    REQUIRE(image.width() == 2);
    REQUIRE(image.height() == 2);
    REQUIRE(*image.data() == 42);
    REQUIRE(image.get(0, 0) == 42);
    REQUIRE(image.get(1, 0) == 43);
    REQUIRE(image.get(0, 1) == 44);
    REQUIRE(image.get(1, 1) == 45);
    REQUIRE(image.at(0, 0) == 42);
    REQUIRE(image.at(1, 0) == 43);
    REQUIRE(image.at(0, 1) == 44);
    REQUIRE(image.at(1, 1) == 45);

    REQUIRE_THROWS(image.at(0, 2));
    REQUIRE_THROWS(image.at(2, 0));
    REQUIRE_THROWS(image.at(2, 2));
    REQUIRE_THROWS(image.at(-1, 0));
    REQUIRE_THROWS(image.at(0, -1));
    REQUIRE_THROWS(image.at(-1, -1));
    
    REQUIRE(image.channels() == 1);
    REQUIRE(image.format() == glpp::core::object::image_format_t::red);
    REQUIRE(image.type() == GL_UNSIGNED_BYTE);
}