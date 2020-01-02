#pragma once

#include <glpp/core.hpp>
#include <glm/glm.hpp>
#include <array>

struct cube_vertex_description_t {
	glm::vec3 position;
	glm::vec3 norm;
};

class cube_model_t : public glpp::render::model_t<cube_vertex_description_t> {
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
		m_verticies.reserve(m_verticies.size()+2*base.size());
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
		m_verticies.emplace_back(first);
		m_verticies.emplace_back(second);
		m_verticies.emplace_back(third);

		m_verticies.emplace_back(first);
		m_verticies.emplace_back(third);
		m_verticies.emplace_back(fourth);
	}
};
