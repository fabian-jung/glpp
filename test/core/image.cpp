#include <catch2/catch.hpp>

#include <glpp/core.hpp>
#include <cmath>

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



TEST_CASE("image copy constructor", "[core][unit]") {
    const glm::vec3 c1(0.5, 0.5, 0.5);
    const glm::vec3 c2(0.5, 0.5, 1.0);
    const glm::vec3 c3(0.5, 1.0, 0.5);
    const glm::vec3 c4(1.0, 0.5, 0.5);
    constexpr float offset = 1.0/32.0;
    const float too_small_offset = std::nextafterf(offset, 0);
    const glpp::core::object::image_t<glm::vec3> image1(
        2, 2,
        { 
            c1, c2,
            c3, c4
        }
    );

    const glpp::core::object::image_t<glm::vec3> image2(
        2, 2,
        { 
            c1, c2,
            c3, c4
        }
    );

    REQUIRE(image1 == image2);

    const glpp::core::object::image_t<glm::vec3> image3(
        2, 2,
        { 
            c1, c2,
            c3, c4+glm::vec3(0,0,offset)
        }
    );
    REQUIRE_FALSE(image1==image3);
    REQUIRE_FALSE(image2==image3);

    REQUIRE_FALSE((image1==image3).epsilon(too_small_offset));
    REQUIRE_FALSE((image2==image3).epsilon(too_small_offset));

    REQUIRE((image1==image3).epsilon(offset));
    REQUIRE((image2==image3).epsilon(offset));

    const glm::vec3 offset_vec(offset, -offset, offset);
    const glpp::core::object::image_t<glm::vec3> image4(
        2, 2,
        { 
            c1+offset_vec, c2-offset_vec,
            c3-offset_vec, c4+offset_vec
        }
    );

    REQUIRE_FALSE(image1==image4);
    REQUIRE_FALSE(image2==image4);
    REQUIRE_FALSE(image3==image4);

    REQUIRE_FALSE((image1==image4).epsilon(too_small_offset));
    REQUIRE_FALSE((image2==image4).epsilon(too_small_offset));
    REQUIRE_FALSE((image3==image4).epsilon(too_small_offset));

    REQUIRE((image1==image4).epsilon(offset));
    REQUIRE((image2==image4).epsilon(offset));
    REQUIRE((image4==image4).epsilon(offset));
}

TEST_CASE("image copy assignment", "[core][unit]") {
  using ubyte = unsigned char;
    const glpp::core::object::image_t<ubyte> image1 (
        2, 2,
        { 
            42, 43,
            44, 45
        }
    );

    glpp::core::object::image_t<ubyte> image2;
    REQUIRE_FALSE(image2==image1);
    image2 = image1;
    REQUIRE(image2==image1);
}

TEST_CASE("image move constructor", "[core][unit]") {
    using ubyte = unsigned char;
    const glpp::core::object::image_t<ubyte> reference (
        2, 2,
        { 
            42, 43,
            44, 45
        }
    );

    glpp::core::object::image_t<ubyte> image1(reference);
    REQUIRE(image1==reference);
    glpp::core::object::image_t<ubyte> image2(std::move(image1));
    REQUIRE(image2==reference);
}

TEST_CASE("image move assignment", "[core][unit]") {
    using ubyte = unsigned char;
    const glpp::core::object::image_t<ubyte> reference (
        2, 2,
        { 
            42, 43,
            44, 45
        }
    );

    glpp::core::object::image_t<ubyte> image1(reference);
    REQUIRE(image1==reference);

    glpp::core::object::image_t<ubyte> image2;
    REQUIRE_FALSE(image2==reference);

    image2 = std::move(image1);
    REQUIRE(image2==reference);
}

TEST_CASE("image conversion constructor", "[core][unit]") {
    const glpp::core::object::image_t<float> f_image {
        2, 2, 
        {
            1.0f, 0.0f,
            0.5f, 0.25f
        }
    };

    const glpp::core::object::image_t<std::uint8_t> ub_image { f_image };
    REQUIRE(ub_image.width() == f_image.width());
    REQUIRE(ub_image.height() == f_image.height());
    REQUIRE(ub_image.get(0, 0) == std::numeric_limits<std::uint8_t>::max());
    REQUIRE(ub_image.get(1, 0) == std::numeric_limits<std::uint8_t>::min());
    REQUIRE(ub_image.get(0, 1) == std::numeric_limits<std::uint8_t>::max()/2+1);

    const glpp::core::object::image_t<std::int8_t> b_image { ub_image };
    REQUIRE(b_image.width() == ub_image.width());
    REQUIRE(b_image.height() == ub_image.height());
    REQUIRE(b_image.get(0, 0) == std::numeric_limits<std::int8_t>::max());
    REQUIRE(b_image.get(1, 0) == std::numeric_limits<std::int8_t>::min());
    REQUIRE(b_image.get(0, 1) == 0);

    const glpp::core::object::image_t<std::uint16_t> ui16_image { b_image };
    REQUIRE(ui16_image.width() == b_image.width());
    REQUIRE(ui16_image.height() == b_image.height());
    REQUIRE(ui16_image.get(0, 0) == std::numeric_limits<std::uint16_t>::max());
    REQUIRE(ui16_image.get(1, 0) == std::numeric_limits<std::uint16_t>::min());

    const glpp::core::object::image_t<double> d_image { ui16_image };
    REQUIRE(d_image.width() == ui16_image.width());
    REQUIRE(d_image.height() == ui16_image.height());
    REQUIRE(d_image.get(0, 0) == 1.0);
    REQUIRE(d_image.get(1, 0) == 0.0);
    
    const glpp::core::object::image_t<float> f_image_2 { d_image };
    REQUIRE(f_image_2.width() == d_image.width());
    REQUIRE(f_image_2.height() == d_image.height());
    REQUIRE(f_image_2.get(0, 0) == 1.0f);
    REQUIRE(f_image_2.get(1, 0) == 0.0f);

    REQUIRE((f_image_2 == f_image).epsilon(1.0f/256.0f));    
}

TEST_CASE("image<float> resize up", "[core][unit]") {
    const glpp::core::object::image_t<float> f_image {
        2, 2, 
        {
            1.0f, 0.0f,
            0.5f, 0.25f
        }
    };

    const auto f_resized = f_image.resize(4, 4);
    const glpp::core::object::image_t<float> f_reference {
        4, 4, 
        {
            1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.25f,  0.25f,
            0.5f, 0.5f, 0.25f,  0.25f
        }
    };
    REQUIRE(f_resized == f_reference);
}


TEST_CASE("image<vec3> resize up", "[core][unit]") {
    const glpp::core::object::image_t<glm::vec3> f_image {
        2, 2, 
        {
            glm::vec3{1.0f}, glm::vec3{0.0f},
            glm::vec3{0.5f}, glm::vec3{0.25f}
        }
    };

    const auto f_resized = f_image.resize(4, 4);
    const glpp::core::object::image_t<glm::vec3> f_reference {
        4, 4, 
        {
            glm::vec3{1.0f}, glm::vec3{1.0f}, glm::vec3{0.0f}, glm::vec3{0.0f},
            glm::vec3{1.0f}, glm::vec3{1.0f}, glm::vec3{0.0f}, glm::vec3{0.0f},
            glm::vec3{0.5f}, glm::vec3{0.5f}, glm::vec3{0.25f},  glm::vec3{0.25f},
            glm::vec3{0.5f}, glm::vec3{0.5f}, glm::vec3{0.25f},  glm::vec3{0.25f}
        }
    };
    REQUIRE(f_resized == f_reference);
}


TEST_CASE("image<float> resize down", "[core][unit]") {
    const glpp::core::object::image_t<float> f_image {
        4, 4, 
        {
            1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.25f, 0.25f,
            0.5f, 0.5f, 0.25f, 0.25f
        }
    };

    const glpp::core::object::image_t<float> f_reference {
        2, 2, 
        {
            1.0f, 0.0f,
            0.5f, 0.25f
        }
    };

    const auto f_resized = f_image.resize(2, 2);
    REQUIRE(f_resized == f_reference);
}

TEST_CASE("image<vec3> resize down", "[core][unit]") {
    const glpp::core::object::image_t<glm::vec3> f_image {
        4, 4, 
        {
            glm::vec3{1.0f}, glm::vec3{1.0f}, glm::vec3{0.0f}, glm::vec3{0.0f},
            glm::vec3{1.0f}, glm::vec3{1.0f}, glm::vec3{0.0f}, glm::vec3{0.0f},
            glm::vec3{0.5f}, glm::vec3{0.5f}, glm::vec3{0.25f},  glm::vec3{0.25f},
            glm::vec3{0.5f}, glm::vec3{0.5f}, glm::vec3{0.25f},  glm::vec3{0.25f}
        }
    };

    const glpp::core::object::image_t<glm::vec3> f_reference {
        2, 2, 
        {
            glm::vec3{1.0f}, glm::vec3{0.0f},
            glm::vec3{0.5f}, glm::vec3{0.25f}
        }
    };

    const auto f_resized = f_image.resize(2, 2);
    REQUIRE(f_resized == f_reference);
}