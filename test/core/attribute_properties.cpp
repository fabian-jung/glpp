#include <catch2/catch_all.hpp>

#include <glpp/core.hpp>
#include <type_traits>

TEST_CASE("attribute_properties basic checks", "[core][unit]") {
    REQUIRE(std::is_same_v<glpp::core::object::attribute_properties<glm::vec3>::value_type, float>);
    REQUIRE(std::is_same_v<glpp::core::object::attribute_properties<glm::dvec3>::value_type, double>);
    REQUIRE(std::is_same_v<glpp::core::object::attribute_properties<glm::ivec3>::value_type, int>);
    //REQUIRE(std::is_same_v<glpp::object::attribute_properties<glm::uvec3>::value_type, unsigned>);

    REQUIRE(glpp::core::object::attribute_properties<glm::vec3>::type == GL_FLOAT);
    REQUIRE(glpp::core::object::attribute_properties<glm::ivec3>::type == GL_INT);
    REQUIRE(glpp::core::object::attribute_properties<glm::uvec3>::type == GL_UNSIGNED_INT);
    REQUIRE(glpp::core::object::attribute_properties<glm::dvec3>::type == GL_DOUBLE);

    REQUIRE(glpp::core::object::attribute_properties<float>::elements_per_vertex == 1);
    REQUIRE(glpp::core::object::attribute_properties<glm::vec2>::elements_per_vertex == 2);
    REQUIRE(glpp::core::object::attribute_properties<glm::dvec3>::elements_per_vertex == 3);
    REQUIRE(glpp::core::object::attribute_properties<glm::vec4>::elements_per_vertex == 4);
}