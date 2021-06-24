#include <iostream>
#include <fstream>

#include <glpp/system.hpp>
#include <glpp/core.hpp>
#include <glpp/text.hpp>

int main() {
	glpp::system::window_t window(600, 600, "example", glpp::system::vsync_t::off);
	window.set_input_mode(glpp::system::input_mode_t::wait);

	glpp::text::font_t font {"Hack-Regular.ttf", 64};

	glpp::text::writer_t writer{ font };

	auto renderer = writer.renderer();

	glpp::core::render::view_t view {
		writer.write(
			glpp::text::label_t{
				{2.0, 2.0},
				{0.0, 0.0},
				glpp::text::horizontal_alignment_t::center,
				glpp::text::vertical_alignment_t::center
			},
			std::string("Hello World!")
		)
	};

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	window.enter_main_loop([&]() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});


	return 0;
}

