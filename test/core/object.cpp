#include <catch2/catch.hpp>

#include <glpp/core.hpp>

TEST_CASE("object_t calls destructor once", "[core, unit]") {
	constexpr GLuint id = 1337;
    int destructor_calls = 0;
    {
        const glpp::object_t object(
            id,
            [&] (const GLuint captured_id) {
                ++destructor_calls;
                REQUIRE(captured_id == id);
            }
        );
        REQUIRE(object.id() == id);
    }
    REQUIRE(destructor_calls == 1);
}

TEST_CASE("object_t calls destructor once after move", "[core, unit]") {
	constexpr GLuint id = 1337;
    int destructor_calls = 0;
    {
        glpp::object_t object(
            id,
            [&] (const GLuint captured_id) {
                ++destructor_calls;
                REQUIRE(captured_id == id);
            }
        );
        REQUIRE(object.id() == id);

        glpp::object_t object2(std::move(object));
        REQUIRE(object2.id() == id);
    }
    REQUIRE(destructor_calls == 1);
}
