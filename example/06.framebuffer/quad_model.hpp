#pragma once

#include <glm/glm.hpp>
#include <glpp/core.hpp>

struct quad_vertex_description_t {
	glm::vec2 position;
	glm::vec2 tex;
};

class quad_model_t : public glpp::render::model_t<quad_vertex_description_t> {
public:
	quad_model_t(std::initializer_list<std::pair<glm::vec2, glm::vec2>> list) {
		for(const auto& p : list) {
			add_quad(p.first, p.second);
		}
	}

	void add_quad(
		const quad_vertex_description_t& first,
		const quad_vertex_description_t& second,
		const quad_vertex_description_t& third,
		const quad_vertex_description_t& fourth
	) {
		m_verticies.reserve(m_verticies.size()+6);
		m_verticies.emplace_back(first);
		m_verticies.emplace_back(second);
		m_verticies.emplace_back(third);

		m_verticies.emplace_back(first);
		m_verticies.emplace_back(third);
		m_verticies.emplace_back(fourth);
	}

	void add_quad(const glm::vec2& position, const glm::vec2& dimension) {
		const auto left_down = position - 0.5f*dimension;
		add_quad(
			quad_vertex_description_t{left_down, {0.0, 0.0}},
			quad_vertex_description_t{left_down+glm::vec2(dimension.x, 0.0), {1.0, 0.0}},
			quad_vertex_description_t{left_down+dimension, {1.0, 1.0}},
			quad_vertex_description_t{left_down+glm::vec2(0.0, dimension.y), {0.0, 1.0}}
		);
	}
};
