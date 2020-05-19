#pragma once

#include <glpp/ui/ui.hpp>
#include <glpp/text.hpp>
#include <string>

namespace glpp::ui::element {

template <class CharT>
struct label_t {
	std::basic_string<CharT> text;
	const text::font_t& font;
	glm::vec4 tint { 1.0f };
	text::horizontal_alignment_t halign = text::horizontal_alignment_t::origin;
	text::vertical_alignment_t valign = text::vertical_alignment_t::baseline;

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2 bottom_left,
		const glm::vec2 top_right,
		const ui_base_t::textures_t& textures
	) {
		const int tex_id = textures.offset(font.texture_slot().id());
		glm::vec2 origin;
		switch(halign) {
			case text::horizontal_alignment_t::right:
				origin.x = top_right.x;
				break;
			case text::horizontal_alignment_t::center:
				origin.x = glm::mix(top_right.x, bottom_left.x, 0.5f);
				break;
			default:
				origin.x = bottom_left.x;
		}

		switch(valign) {
			case text::vertical_alignment_t::top:
				origin.y = top_right.y;
				break;
			case text::vertical_alignment_t::center:
				origin.y = glm::mix(top_right.y, bottom_left.y, 0.5f);
				break;
			default:
				origin.y = bottom_left.y;
		}

		const auto glyphs = text::writer_t{ font }.write(
			text::label_t {
				top_right-bottom_left,
				origin,
				halign,
				valign
			},
			text
		);
		std::transform(
			glyphs.begin(),
			glyphs.end(),
			std::back_inserter(model),
			[tex_id, color = tint](const auto& vertex){
				return ui_base_t::vertex_description_t { vertex.position, vertex.tex, tex_id, color };
			}
		);
	}

	void textures(ui_base_t::textures_t& textures) {
		textures.insert(font.texture_slot().id());
	}

	template <class Eve, class... Args>
	void act(Eve, const Args&...) {
	}

};

template <class CharT, class... Args>
label_t(std::basic_string<CharT>, const text::font_t&, glm::vec4, Args&&...) -> label_t<CharT>;

template <class CharT, class... Args>
label_t(std::basic_string<CharT>, const text::font_t&, Args&&...) -> label_t<CharT>;

}
