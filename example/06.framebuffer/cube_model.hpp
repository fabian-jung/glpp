#pragma once

#include <glpp/core.hpp>
#include <glm/glm.hpp>
#include <array>

struct cube_vertex_description_t {
	glm::vec3 position;
	glm::vec3 norm;
};

class cube_model_t : public glpp::core::render::model_t<cube_vertex_description_t> {
public:
	cube_model_t() = default;

	cube_model_t(std::initializer_list<std::pair<glm::vec3, glm::vec3>> list) {
		for(const auto& p : list) {
			add_cube(p.first, p.second);
		}
	}

	void add_cube(glm::vec3 center, glm::vec3 size) {
		const auto offset = 0.5f*size;
		struct base_t {
			glm::vec3 base_one;
			glm::vec3 base_two;
			glm::vec3 normal;
		};

		constexpr std::array<base_t, 6> base{{
			{{1,0,0}, {0,1,0}, {0,0,1}},
			{{1,0,0}, {0,1,0}, {0,0,-1}},
			{{0,1,0}, {0,0,1}, {1,0,0}},
			{{0,1,0}, {0,0,1}, {-1,0,0}},
			{{1,0,0}, {0,0,1}, {0,1,0}},
			{{1,0,0}, {0,0,1}, {0,-1,0}}
		}};
		for(const auto& b : base) {
			add_quad(
				{center+offset*(-b.base_one-b.base_two+b.normal), b.normal},
				{center+offset*( b.base_one-b.base_two+b.normal), b.normal},
				{center+offset*( b.base_one+b.base_two+b.normal), b.normal},
				{center+offset*(-b.base_one+b.base_two+b.normal), b.normal}
			);
		}
	}

private:
	void add_quad(
		const cube_vertex_description_t& first,
		const cube_vertex_description_t& second,
		const cube_vertex_description_t& third,
		const cube_vertex_description_t& fourth
	) {
		emplace_back(first);
		emplace_back(second);
		emplace_back(third);

		emplace_back(first);
		emplace_back(third);
		emplace_back(fourth);
	}
};

namespace glpp::core::render {
	template <>
	struct model_traits<cube_model_t> : public model_traits<glpp::core::render::model_t<cube_vertex_description_t>> {};
}
