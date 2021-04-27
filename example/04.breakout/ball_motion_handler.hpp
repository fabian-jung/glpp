#pragma once
#include <glpp/core/object/texture.hpp>
#include <chrono>
#include <glm/glm.hpp>
#include <vector>

class ball_motion_handler_t {
	using clock_t = std::chrono::high_resolution_clock;
	using seconds = std::chrono::duration<float>;
	enum class axis_t {
		x, y
	};

	struct line_t {
		glm::vec2 begin, end;
	};

	struct hit_t {
		bool happend = false;
		float time = std::numeric_limits<float>::max();
		axis_t axis = axis_t::x;
		size_t x = 0,y = 0;
	};

public:
	ball_motion_handler_t();

	void start();
	void update();

	glm::vec2 position() const ;
	constexpr float size() const;
	constexpr static float radius();


	float slider_width() const;
	void set_slider(float x);
	float get_slider() const;

	glpp::core::object::image_t<GLubyte>& level();
	glpp::core::object::texture_t& level_texture();

private:
	float m_slider_x;
	bool m_running = false;
	glpp::core::object::image_t<GLubyte> m_level;
	glpp::core::object::texture_t m_level_tex;
	constexpr static float m_size = .05;
	clock_t::time_point m_last_update = clock_t::now();
	glm::vec2 m_direction = glm::normalize(glm::vec2(1, .9));
	glm::vec2 m_position = glm::vec2(0, -.9);
	float m_speed = 0;

	float step(float time);
	glm::vec2 intersect(const line_t line1, const line_t line2);
	void deactivate_tile(size_t x, size_t y);
	hit_t check_tile_hit(size_t x, size_t y, float time);
	bool is_active(size_t x, size_t y);
};

constexpr float ball_motion_handler_t::size() const {
	return m_size;
}

constexpr float ball_motion_handler_t::radius() {
	return m_size/2;
}
