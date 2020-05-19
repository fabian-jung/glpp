#include "glpp/ui/element/image.hpp"
#include <algorithm>

namespace glpp::ui::element {

image_t::image_t(const glpp::object::texture_slot_t& texture, glm::vec4 tint) :
	texture(&texture),
	tint(tint)
{}

void image_t::triangulate(
	ui_base_t::model_t& model,
	const glm::vec2 bottom_left,
	const glm::vec2 top_right,
	const ui_base_t::textures_t& textures
) {
	GLint tex_id = textures.offset(texture->id());
	using vd = ui_base_t::vertex_description_t;
	model.emplace_back(vd{bottom_left, glm::vec2{0,0}, tex_id, tint});
	model.emplace_back(vd{{bottom_left.x, top_right.y}, glm::vec2{0,1}, tex_id, tint});
	model.emplace_back(vd{top_right, glm::vec2{1,1}, tex_id, tint});

	model.emplace_back(vd{bottom_left, glm::vec2{0,0}, tex_id, tint});
	model.emplace_back(vd{top_right, glm::vec2{1,1}, tex_id, tint});
	model.emplace_back(vd{{top_right.x, bottom_left.y}, glm::vec2{1,0}, tex_id, tint});
}

void image_t::textures(ui_base_t::textures_t& textures) {
	textures.insert(texture->id());
}

}
