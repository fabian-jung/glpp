 #include <glpp/core.hpp>
 #include <glpp/system.hpp>
 #include <glpp/ui.hpp>

 #include <iostream>

#include <glm/gtc/random.hpp>

struct my_button_t : public glpp::ui::element::button_t<glpp::ui::element::image_t> {
	using image_t = glpp::ui::element::image_t;

	my_button_t(const glpp::object::texture_slot_t& slot, std::function<void()> action) :
		button_t(
			image_t { slot },
			action,
			[](auto& frame){ frame.tint = glm::vec4(glm::vec3(0.8f), 1.0f); },
			[](auto& frame){ frame.tint = glm::vec4(1.0f); },
			[](auto& frame){ frame.tint = glm::vec4(glm::vec3(0.8f), 1.0f); },
			[](auto& frame){ frame.tint = glm::vec4(1.0f); }
		)
	{}

	using glpp::ui::element::button_t<glpp::ui::element::image_t>::action;
};

int main(int, char*[]) {
 	glpp::system::window_t window(800, 800, "example", glpp::system::vsync_t::off);
 	window.set_input_mode(glpp::system::input_mode_t::wait);
	window.set_aspect_ratio(1.0f);
 	using namespace glpp::ui;
	using namespace glpp::ui::element;

 	glpp::text::font_t font {"Hack-Regular.ttf", 160};


	glpp::object::texture_t add_icon {
 		glpp::object::image_t<glm::vec4>{
			"add.png"
		}
	};

	glpp::object::texture_t remove_icon {
		glpp::object::image_t<glm::vec4>{
			"remove.png"
		}
	};

 	auto add_slot = add_icon.bind_to_texture_slot();
	auto remove_slot = remove_icon.bind_to_texture_slot();

	ui_t ui {
		flow_t {
			flow_direction_t::horizontal,
			flow_t {
				flow_direction_t::vertical,
				box_t { glm::vec2(1.8), my_button_t(add_slot, [](){}) },
				box_t { glm::vec2(1.8), my_button_t(remove_slot, [](){}) }
			},
			flow_list_t<box_t<frame_t>> {
				flow_direction_t::vertical
			},
		},
		window.input_handler()
	};
	auto& add_button = ui.widget.get<0>().get<0>().child;
	auto& remove_button = ui.widget.get<0>().get<1>().child;
	auto& list = ui.widget.get<1>();
	add_button.action = [&](){
		list.elements.emplace_back(box_t{glm::vec2(1.8), frame_t {glm::vec4(1.0f)}});
	};
	remove_button.action = [&](){
		if(list.elements.size() > 0) {
			list.elements.pop_back();
		}
	};
 	glClearColor(0.2, 0.2, 0.2, 1.0);
 	glDisable(GL_DEPTH_TEST);
 	glEnable(GL_BLEND);
 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

 	window.enter_main_loop([&](){
 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		ui.update();
		ui.render();
	});
	return 0;
 }
