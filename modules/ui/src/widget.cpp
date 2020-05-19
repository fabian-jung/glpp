#include "glpp/ui/element/widget.hpp"

namespace glpp::ui::element {

widget_t::widget_t(const widget_t& cpy) :
	child(cpy.child->clone())
{}

widget_t& widget_t::operator=(const widget_t& cpy) {
	if(cpy.child) {
		child.reset(cpy.child->clone());
	} else {
		child.reset();
	}
	return *this;
}

void widget_t::textures(ui_base_t::textures_t& textures) {
	if(child) child->textures(textures);
}

void widget_t::triangulate(
	ui_base_t::model_t& model,
	const glm::vec2& bottom_left,
	const glm::vec2& top_right,
	const ui_base_t::textures_t& texture
) {
	if(child) child->triangulate(model, bottom_left, top_right, texture);
}

}
