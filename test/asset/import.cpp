#include <catch2/catch.hpp>
#include <glpp/asset.hpp>
#include <glpp/test/context.hpp>
#include <vector>
#include <string>

using namespace glpp::asset;
using namespace glpp::test;
using namespace glpp::core::object;

TEST_CASE("Loading scene with non existant file throws", "[asset][unit]") {
    REQUIRE_THROWS(scene_t("not_existant.obj"));
}

TEST_CASE("Loading cube scene from .fbx", "[asset][unit]") {
    
    std::vector<std::string> file_types {
        "fbx",
        "glb"
    };

    for(const auto& ending : file_types) {
        DYNAMIC_SECTION("Loading from cube." << ending) {
            const auto file = "cube."+ending;
            const scene_t scene(file.c_str());
            REQUIRE(scene.meshes.size() == 1);
            const auto& cube_mesh = scene.meshes.front();
            constexpr auto cube_faces = 6;
            constexpr auto triangles_per_face = 2;
            constexpr auto verts_per_face = 3;
            constexpr auto faces = cube_faces*triangles_per_face;
            REQUIRE(cube_mesh.model.indicies.size() == verts_per_face*faces);
            REQUIRE(cube_mesh.model.verticies.size() == cube_faces*4);
            for(const auto& vertex : cube_mesh.model.verticies) {
                REQUIRE(glm::all(glm::lessThanEqual(vertex.position, glm::vec3(1.0))));
                REQUIRE(glm::all(glm::greaterThanEqual(vertex.position, glm::vec3(-1.0))));
                REQUIRE(glm::length(vertex.normal) == 1.0f);
                REQUIRE(glm::all(glm::lessThanEqual(vertex.tex, glm::vec2(1.0))));
                REQUIRE(glm::all(glm::greaterThanEqual(vertex.tex, glm::vec2(0.0))));
            }
            // REQUIRE(scene.ambient_lights.size() == 0);
            // REQUIRE(scene.point_lights.size() == 0);
            // REQUIRE(scene.spot_lights.size() == 0);
            // REQUIRE(scene.directional_lights.size() == 1);
        }
    }
}