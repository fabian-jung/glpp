#include "ball_motion_handler.hpp"


ball_motion_handler_t::ball_motion_handler_t() :
	m_level("level.png"),
	m_level_tex(m_level)
{}

void ball_motion_handler_t::update() {
	const auto now = clock_t::now();
	const auto delta = std::chrono::duration_cast<seconds>(now - m_last_update).count();
	m_last_update = now;
	float remaining;
	do {
		remaining = step(delta);
	} while(remaining > 0);
}

float ball_motion_handler_t::step(float time) {
	const float top = 1-radius(), bottom = -1+radius()+.075, left = -1+radius(), right = 1-radius();
	std::vector<line_t> bounds(2);
	if(m_direction.x<0)
	{
		bounds[0] = {glm::vec2(left, bottom), glm::vec2(left, top)};
	}  else {
		bounds[0] = {glm::vec2(right, bottom), glm::vec2(right, top)};
	}
	if(m_direction.y>0) {
		bounds[1] = {glm::vec2(left, top), glm::vec2(right, top)};
	} else {
		bounds[1] = {glm::vec2(m_slider_x-0.5*slider_width(), bottom), glm::vec2(m_slider_x+0.5*slider_width(), bottom)};
	}

	auto end = m_position+time*m_speed*m_direction;
	const line_t flight_path = {m_position, end};
	for(auto i = 0u; i < bounds.size(); ++i) {
		const auto factors = intersect(bounds[i], flight_path);
		if( factors[0] <= 1 && factors[1] <= 1 &&
			factors[0] >= 0 && factors[1] >= 0
		) {
			auto time_to_colision = time*factors[1];
			m_position = glm::mix(m_position, end, factors[1]);
			glm::vec2 f = i%2 == 0 ? glm::vec2(-1,1) : glm::vec2(1, -1);
			m_direction *= f;
			return time-time_to_colision;
		}
	}

	hit_t rolling_minimum{false, time};
	for(size_t y = 0; y < m_level.height(); ++y) {
		for(size_t x = 0; x < m_level.width(); ++x) {
			const auto hit = check_tile_hit(x, y, time);
			if(hit.happend && hit.time < rolling_minimum.time) {
				rolling_minimum = hit;
			}
		}
	}

	if(!rolling_minimum.happend) {
		m_position = end;
		return 0;
	} else {
		m_position += rolling_minimum.time*m_speed*m_direction;
		deactivate_tile(rolling_minimum.x, rolling_minimum.y);
		if(rolling_minimum.axis == axis_t::x) {
			m_direction.y *= -1;
		} else {
			m_direction.x *= -1;
		}
		return time - rolling_minimum.time;
	}
}

glm::vec2 ball_motion_handler_t::intersect(const line_t line1, const line_t line2) {
	const glm::vec2 dir1 = line1.end-line1.begin;
	const glm::vec2 dir2 = line2.end-line2.begin;

	const glm::vec2 ac = (line1.begin-line2.begin);
	const glm::mat2 M (
		-dir1, dir2
	);
	return glm::inverse(M)*ac;
}

ball_motion_handler_t::hit_t
ball_motion_handler_t::check_tile_hit(size_t x, size_t y, float time) {
	if(!is_active(x,y)) return hit_t {};

	const float bottom = 2.0f*y/m_level.height()-1 -radius();
	const float left = 2.0f*x/m_level.width()-1 -radius();
	const float top = bottom + 2.0f/m_level.height() + size();
	const float right = left + 2.0f/m_level.width() + size();

	std::array<line_t,2> bounds;
	if(m_direction.x<0)
	{
		bounds[0] = {glm::vec2(right, bottom), glm::vec2(right, top)};
	}  else {
		bounds[0] = {glm::vec2(left, bottom), glm::vec2(left, top)};
	}
	if(m_direction.y>0) {
		bounds[1] = {glm::vec2(left, bottom), glm::vec2(right, bottom)};
	} else {
		bounds[1] = {glm::vec2(left, top), glm::vec2(right, top)};
	}

	auto end = m_position+time*m_speed*m_direction;
	const line_t flight_path = {m_position, end};
	for(auto i = 0u; i < bounds.size(); ++i) {
		const auto factors = intersect(bounds[i], flight_path);
		if( factors[0] <= 1 && factors[1] <= 1 &&
			factors[0] >= 0 && factors[1] >= 0
		) {
			auto time_to_colision = time*factors[1];
			return {true, time_to_colision, i%2==0 ? axis_t::y : axis_t::x, x, y};
		}
	}
	return {};
}
bool ball_motion_handler_t::is_active(size_t x, size_t y) {
	return m_level.get(x, y) > 0;
}

void ball_motion_handler_t::deactivate_tile(size_t x, size_t y) {
	m_level.get(x,y) = 0;
	m_level_tex.update(m_level, x, y, 1, 1);
}

glm::vec2 ball_motion_handler_t::position() const {
	return m_position;
}

constexpr float ball_motion_handler_t::size() const {
	return m_size;
}

constexpr float ball_motion_handler_t::radius() {
	return m_size/2;
}

void ball_motion_handler_t::start() {
	if(!m_running) {
		m_running = true;
		m_speed = .5;
	}
}

void ball_motion_handler_t::set_slider(float x) {
	m_slider_x = x;
}

float ball_motion_handler_t::slider_width() const {
	return 0.25;
}

float ball_motion_handler_t::get_slider() const {
	return m_slider_x;
}

glpp::object::image_t<GLubyte>& ball_motion_handler_t::level() {
	return m_level;
}

glpp::object::texture_t& ball_motion_handler_t::level_texture() {
	return m_level_tex;
}

