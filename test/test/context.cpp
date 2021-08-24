#include <catch2/catch.hpp>
#include <glpp/testing/context.hpp>

TEST_CASE("single test context construction and teardown","[test][unit][xorg]") {
    SECTION("window driver") {
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
    }
    SECTION("offscreen driver") {
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
    }
}

TEST_CASE("three test context constructions and teardowns","[test][unit][xorg]") {
    SECTION("window driver") {
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
    }
    SECTION("offscreen driver") {
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
        REQUIRE_NOTHROW(glpp::test::context_t<glpp::test::offscreen_driver_t>(16,16));
    }
}

TEST_CASE("test context clear color buffer","[test][unit][xorg]") {
    glpp::test::context_t<glpp::test::offscreen_driver_t> context(1,1);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    const auto black_pixel = context.swap_buffer();
    REQUIRE(black_pixel.at(0,0) == glm::vec3(0));

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    const auto white_pixel = context.swap_buffer();
    REQUIRE(white_pixel.at(0,0) == glm::vec3(1));
}