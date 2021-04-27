#include <glpp/system.hpp>
#include <glpp/ui.hpp>
#include <random>

using namespace glpp::ui;
using namespace glpp::ui::element;
	constexpr size_t board_size = 5;

using board_base_t =
	flow_list_t<
		flow_list_t<
			mouse_action_t<
				image_t,
				std::function<void(mouse_event_t::press_t, glm::vec2)>
			>,
			static_flow_policy_t<board_size>
		>,
		static_flow_policy_t<board_size>
	>;

struct board_t : public board_base_t
{
	const auto& tex_slot() {
		static glpp::core::object::texture_t lights_on {
			glpp::core::object::image_t<glm::vec4>{"Light.png"}
		};
		static auto slot = lights_on.bind_to_texture_slot();
		return slot;
	}

	auto& operator()(size_t i, size_t j) {
		return elements[i].elements[j];
	};

	board_t(board_t&& move) :
		board_base_t(move)
	{
		for(auto i = 0u; i < board_size; ++i) {
			for(auto j = 0u; j < board_size; ++j) {
				(*this)(i, j).action = [i,j, this](mouse_event_t::press_t, glm::vec2){
					click(i, j);
				};
			}
		}
	}
	board_t() :
		board_base_t {
			static_flow_policy_t<board_size>{},
			flow_direction_t::horizontal,
			{
				static_flow_policy_t<board_size>{},
				flow_direction_t::vertical,
				mouse_action_t {
					image_t { tex_slot() },
					std::function<void(mouse_event_t::press_t, glm::vec2)>{}
				}
			}
		}
	{
		init();

		for(auto i = 0u; i < board_size; ++i) {
			for(auto j = 0u; j < board_size; ++j) {
				(*this)(i, j).action = [i,j, this](mouse_event_t::press_t, glm::vec2){
					click(i, j);
				};
			}
		}
	}

	void init() {
		static auto turns = 0;
		++turns;
		std::random_device rdev;
		std::mt19937 rgen(rdev());
		std::uniform_int_distribution<int> idist(0, board_size-1);
		for(auto i = 0; i < turns; ++i) {
			click(idist(rgen), idist(rgen));
		}
	}

	void click(size_t i, size_t j) {
		toggle(i,j);
		if(i>0) toggle(i-1,j);
		if(i<board_size-1) toggle(i+1,j);
		if(j>0) toggle(i,j-1);
		if(j<board_size-1) toggle(i,j+1);
	}
	void toggle(size_t i, size_t j) {
		auto& field = (*this)(i, j);
		auto& alpha = field.child.tint.a;
		alpha = 1.0f - alpha;
	}

	bool victory() {
		for(auto i = 0u; i < board_size; ++i) {
			for(auto j = 0u; j < board_size; ++j) {
				if((*this)(i, j).child.tint.a < 0.5f) {
					return false;
				}
			}
		}
		return true;
	};

};

int main(int, char*[]) {
	glpp::system::window_t window(800, 800, "Lights out", glpp::system::vsync_t::off);
	window.set_aspect_ratio(1);
	window.set_input_mode(glpp::system::input_mode_t::wait);

	glpp::text::font_t font {"Hack-Regular.ttf", 128};

	ui_t ui {
		board_t{},
		window.input_handler()
	};
	auto& board = ui.widget;

	ui_t victory_screen {
		mouse_action_t {
			box_t {
				glm::vec2(1.5f),
				flow_t {
					flow_direction_t::vertical,
					label_t {
						std::string("Click to restart."),
						font
					},
					label_t {
						std::string("Victory"),
						font
					}
				}
			},
			[&](mouse_event_t::press_t, auto){
				board.init();
				ui.register_mouse();
			},
		},
		window.input_handler()
	};

	ui.register_mouse();

	using glpp::system::mouse_button_t;
	using glpp::system::action_t;

	glClearColor(0.2, 0.2, 0.2, 1.0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	window.enter_main_loop([&](){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if(!board.victory()) {
			ui.update();
			ui.render();
		} else {
			victory_screen.register_mouse();
			victory_screen.render();
		}
	});
	return 0;
}
