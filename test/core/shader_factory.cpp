#include <catch2/catch_all.hpp>
#include <glpp/core/object/shader_factory.hpp>
#include <filesystem>
#include <glm/glm.hpp>

using namespace glpp::core::object;

TEST_CASE("shader_factory_t construction", "[core][unit]") {

    SECTION("with std::stringstream") {
        std::stringstream sstream;
        sstream << "code";
        shader_factory_t factory(std::move(sstream));
        REQUIRE(factory.code() == "code");
    }

    SECTION("with const std::stringstream&") {
        std::stringstream sstream;
        sstream << "code";
        shader_factory_t factory(sstream);
        REQUIRE(factory.code() == "code");
    }

    SECTION("with std::string") {
        shader_factory_t factory("code");
        REQUIRE(factory.code() == "code");
    }

}

TEST_CASE("shader_factory_t construction from files", "[core][unit]") {

    const std::string seed = "xIk7gTNKCOX3G8GlwPR0-";
    const std::string filename = seed+"_code.txt";

    REQUIRE(!std::filesystem::exists(filename));
    {
        std::ofstream file { filename };
        file << "code";
    }

    SECTION("with const std::ifstream&&") {

        shader_factory_t factory(std::ifstream{ filename });
        std::filesystem::remove(filename);

        REQUIRE(factory.code() == "code");
    
    }


    SECTION("with const std::ifstream&") {

        std::ifstream code(filename);
        shader_factory_t factory(code);
       
        std::filesystem::remove(filename);

        REQUIRE(factory.code() == "code");

    }

    REQUIRE(!std::filesystem::exists(filename));

}

TEST_CASE("shader_factory_t::set()", "[core][unit]") {
    shader_factory_t factory("<var>");

    SECTION("with glm::vec3 value") {
        factory.set("<var>", glm::vec3(1.0f, 0.5f, 0.3f));
        REQUIRE(factory.code() == "vec3( 1.000000, 0.500000, 0.300000)");

    }

    SECTION("with glm::ivec3 value") {
        factory.set("<var>", glm::ivec4(1, 2, 3, 4));
        REQUIRE(factory.code() == "vec4( 1, 2, 3, 4)");

    }

    SECTION("with glm::uvec2 value") {
        factory.set("<var>", glm::uvec2(1, 2));
        REQUIRE(factory.code() == "vec2( 1, 2)");

    }

    SECTION("with uchar value") {
        factory.set("<var>", static_cast<std::uint8_t>(17));
        REQUIRE(factory.code() == "17");

    }

    SECTION("with recursion") {
        factory.set("<var>", "1, <var>");
        factory.set("<var>", "2, <var>");
        factory.set("<var>", "3");
        REQUIRE(factory.code() == "1, 2, 3");
    }
}