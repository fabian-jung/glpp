#include <catch2/catch.hpp>

#include <glpp/core/object/texture_atlas.hpp>
#include <glpp/gl/context.hpp>

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