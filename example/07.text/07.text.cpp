

#include <iostream>
#include <fstream>

#include <glpp/system.hpp>
#include <glpp/core.hpp>
#include <glpp/text.hpp>

int main() {
	glpp::system::window_t window(600, 600, "example", glpp::system::vsync_t::off);
	window.set_input_mode(glpp::system::input_mode_t::wait);

	glpp::text::writer_t writer{
		{"/usr/share/fonts/TTF/Hack-Regular.ttf", 64}
	};

	auto renderer = writer.renderer();

	glpp::render::view_t view {
		writer.write(
			std::string("Hello World!"),
			1.8,
			glpp::text::writer_t::horizontal_alignment_t::center,
			glpp::text::writer_t::vertical_alignment_t::center,
			glpp::text::writer_t::scale_t::x_axis
		),
		&glpp::text::writer_t::vertex_description_t::position,
		&glpp::text::writer_t::vertex_description_t::tex
	};

	glpp::call(glClearColor, 0.2, 0.2, 0.2, 1.0);
	glpp::call(glEnable, GL_BLEND);
	glpp::call(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	window.enter_main_loop([&]() {
		glpp::call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer.render(view);
	});


	return 0;
}

