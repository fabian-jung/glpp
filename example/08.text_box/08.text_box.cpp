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

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::string text;

	const auto model = writer.write(
		glpp::text::text_box_t{
			{1.8, 1.8},
			0.08,
			{ 0, 0 },
			glpp::text::horizontal_alignment_t::center,
			glpp::text::vertical_alignment_t::center,
			glpp::text::paragraph_alignment_t::center
		},
		std::string("Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.")
	);

	glpp::render::view_t view {
		model,
		&glpp::text::writer_t::vertex_description_t::position,
		&glpp::text::writer_t::vertex_description_t::tex
	};

	window.enter_main_loop([&]() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



		renderer.render(view);
	});

	return 0;
}
