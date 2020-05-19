#include "glpp/ui/element/frame.hpp"

namespace glpp::ui::element {

void frame_t::triangulate(
	ui_base_t::model_t& model,
	const glm::vec2 bottom_left,
	const glm::vec2 top_right,
	const ui_base_t::textures_t&
) {
	using vd = ui_base_t::vertex_description_t;
	model.emplace_back(vd{bottom_left, glm::vec2{0,0}, -1, tint});
	model.emplace_back(vd{{bottom_left.x, top_right.y}, glm::vec2{0,1}, -1, tint});
	model.emplace_back(vd{top_right, glm::vec2{1,1}, -1, tint});

	model.emplace_back(vd{bottom_left, glm::vec2{0,0}, -1, tint});
	model.emplace_back(vd{top_right, glm::vec2{1,1}, -1, tint});
	model.emplace_back(vd{{top_right.x, bottom_left.y}, glm::vec2{1,0}, -1, tint});
}

void frame_t::textures(ui_base_t::textures_t&) {
}

}
