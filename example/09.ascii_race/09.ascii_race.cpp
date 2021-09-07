#include <iostream>
#include <fstream>

#include <glpp/system.hpp>
#include <glpp/core.hpp>
#include <glpp/text.hpp>

#include <thread>
#include <algorithm>

class acii_race_t {
public:
	using uniform_description_t = glpp::text::writer_t::uniform_description_t;

	acii_race_t(const glpp::text::font_t& font) :
		m_writer(font),
		m_renderer(m_writer.renderer())
	{
		m_renderer.set_uniform(&uniform_description_t::color, glm::vec4(0.95, 0.95, 0.95, 1.00));
		init();
	}

	void advance() {
		if(!m_gameover) {
			auto pos = m_track.size() > 0 ? m_track.front() : 4;
			if(m_track.size() == track_lines()) {
				pos += 3*(static_cast<float>(rand())/static_cast<float>(RAND_MAX))-1;
				pos = std::clamp(pos, 0, static_cast<int>(track_width()-m_street.size()));
				m_score += 10;
				m_highscore = std::max(m_score, m_highscore);
				if(
					m_car <= m_track[m_car_offset]+1 ||
					m_car >= m_track[m_car_offset]+static_cast<int>(m_street.size())
				) {
					m_gameover = true;
					return;
				}
				if(m_left_pressed) --m_car;
				if(m_right_pressed) ++m_car;
				m_input_handled = true;
			}
			m_track.insert(m_track.begin(), pos);
			m_track.resize(std::min(track_lines(), m_track.size()));
		}
	}

	void render() {
		for(auto i = 0u; i < m_track.size(); ++i) {
			print(line(m_track[i], static_cast<int>(i)==m_car_offset?m_car:-1), i);
		}
		print_score();
		if(m_gameover) {
			print_gameover();
		}
	}

	void press_space() {
		if(m_gameover) {
			init();
		}
	}

	void press_left() {
		m_input_handled = false;
		m_left_pressed = true;
	}

	void release_left() {
		if(!m_input_handled && !m_gameover) {
			--m_car;
		}
		m_left_pressed = false;
	}

	void press_right() {
		if(!m_input_handled && !m_gameover) {
			++m_car;
		}
		m_right_pressed = true;
	}

	void release_right() {
		m_right_pressed = false;
	}

	auto& highscore() {
		return m_highscore;
	}

private:
	constexpr size_t track_lines() const {
		return screen_height-1;
	}

	constexpr size_t track_width() const {
		return screen_width-2;
	}

	void init() {
		m_gameover = false;
		m_track.resize(0);
		m_car = 7;
		m_car_offset = 8;
		m_score = 0;
		srand(0);
	}

	std::string line(int track, int car) {
		std::string s(screen_width, ' ');
// 		s.front() = '|';
// 		s.back() = '|';
		std::copy(m_street.begin(), m_street.end(), s.begin()+track+1);
		if(car > 0) {
			s[car] = '^';
		}
		return s;
	}

	void print(const std::string& s, int y, float x = 0, glpp::text::horizontal_alignment_t halign = glpp::text::horizontal_alignment_t::center) {
		glpp::core::render::view_t view {
			m_writer.write(
				glpp::text::label_t{
					{0,font_size},
					{x,1-1.1*y*font_size},
					halign,
					glpp::text::vertical_alignment_t::top
				},
				s
			)
		};
		m_renderer.render(view);
	}

	void print_score() {
		print(std::to_string(m_score), screen_height-1, 1, glpp::text::horizontal_alignment_t::right);
		print(std::to_string(m_highscore), screen_height-1, -1, glpp::text::horizontal_alignment_t::left);
	}

	void print_gameover() {
		print(std::string("Hit SPACE"), screen_height-1, 0, glpp::text::horizontal_alignment_t::center);
	}

	std::string m_street = "#   #";
	static constexpr size_t screen_width = 17;
	static constexpr size_t screen_height = 13;
	static constexpr float font_size = (1.0/1.11)*(2.0/static_cast<float>(screen_height));
	glpp::text::writer_t m_writer;
	glpp::core::render::renderer_t<glpp::text::writer_t::uniform_description_t> m_renderer;
	bool m_gameover = false;
	std::vector<int> m_track;
	int m_car = 7, m_car_offset = 8;
	int m_score = 0;
	int m_highscore = 0;
	bool m_left_pressed = false, m_right_pressed=false, m_input_handled;
};

int main(int argc, char* argv[]) {
	glpp::system::window_t window(600, 600, "ASCII Race", glpp::system::vsync_t::off);
	std::string config = argc>1? argv[1] : "config.txt";
	window.set_aspect_ratio(1.0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glpp::text::font_t hack {"Hack-Regular.ttf", 64};
	acii_race_t race { hack };
	{
		std::ifstream config_file(config);
		config_file >> race.highscore();
	}
	glClearColor(0.2, 0.2, 0.2, 1.0);

	window.input_handler().set_keyboard_action(glpp::system::key_t::left, glpp::system::action_t::press, [&](int) {
		race.press_left();
	});
	window.input_handler().set_keyboard_action(glpp::system::key_t::left, glpp::system::action_t::release, [&](int) {
		race.release_left();
	});
	window.input_handler().set_keyboard_action(glpp::system::key_t::right, glpp::system::action_t::press, [&](int) {
		race.press_right();
	});

	window.input_handler().set_keyboard_action(glpp::system::key_t::right, glpp::system::action_t::release, [&](int) {
		race.release_right();
	});

	window.input_handler().set_keyboard_action(glpp::system::key_t::space, glpp::system::action_t::press, [&](int) {
		race.press_space();
	});

	window.enter_main_loop([&]() {
		std::this_thread::sleep_for(std::chrono::milliseconds(150));
		glClear(GL_COLOR_BUFFER_BIT);
		race.advance();
		race.render();
	});

	std::ofstream config_file(config);
	config_file << race.highscore();

	return 0;
}

