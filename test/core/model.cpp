#include <catch2/catch.hpp>

#include <glpp/core.hpp>

TEST_CASE("model_traits check for model_t<vec3>", "[core][unit]") {
    using model_t = glpp::core::render::model_t<glm::vec3>;
	const model_t model {
        {0, 0, 0},
        {0, 1, 0},
        {1, 1, 0}
    };

    using traits = glpp::core::render::model_traits<model_t>;
    REQUIRE(traits::buffer_count() == 1);
    REQUIRE(traits::instanced() == false);
    REQUIRE(traits::verticies(model) == model);	
}

TEST_CASE("model_traits check for indexed_model_t<vec3>", "[core][unit]") {
    using model_t = glpp::core::render::indexed_model_t<glm::vec3>;
	const model_t model {
        {
            {0, 0, 0},
            {0, 1, 0},
            {1, 1, 0}
        },
        {0, 1, 2}
    };

    using traits = glpp::core::render::model_traits<model_t>;
    REQUIRE(traits::buffer_count() == 1);
    REQUIRE(traits::instanced() == true);
    REQUIRE(traits::verticies(model) == model.verticies);	
    REQUIRE(traits::indicies(model) == model.indicies);	
}