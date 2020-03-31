#include "glpp/text/writer.hpp"

namespace glpp::text {

writer_t::writer_t(font_t&& atlas) :
	m_font(std::move(atlas))
{}

glpp::object::shader_t writer_t::vertex_shader() {
	return {
		glpp::object::shader_type_t::vertex,
		"#version 330 core\n\
		layout (location = 0) in vec2 pos;\n\
		layout (location = 1) in vec2 tex;\n\
		out vec2 v_tex;\n\
		\n\
		void main()\n\
		{\n\
		v_tex = tex;\n\
		gl_Position = vec4(pos, 0.0, 1.0);\n\
	}"
	};
}

glpp::object::shader_t writer_t::fragment_shader() {
	return {
		glpp::object::shader_type_t::fragment,
		"#version 330 core\n\
		in vec2 v_tex;\n\
		uniform sampler2D glyphs;\n\
		uniform vec4 color = vec4(1, 0.75, 0.3, 1.0);\n\
		out vec4 FragColor;\n\
		\n\
		void main()\n\
		{\n\
		FragColor = vec4(color.rgb, texture(glyphs, v_tex).x);\n\
	}"
	};
}

glpp::render::renderer_t<> writer_t::renderer() {
	glpp::render::renderer_t result {
		vertex_shader(),
		fragment_shader()
	};
	result.set_texture("glyphs", m_font.texture_slot());
	return result;
}

void add_glyph(writer_t::model_t& model, const glm::vec2& origin, const glyph_t& glyph) {
	const auto& bottom_left = origin+glyph.bottom_left;
	const auto top_right = bottom_left+glyph.size;
	const auto& tex_range = glyph.tex_range;
	model.emplace_back(writer_t::vertex_description_t{bottom_left,                  {tex_range[0], 0.0}});
	model.emplace_back(writer_t::vertex_description_t{top_right,                    {tex_range[1], 1.0}});
	model.emplace_back(writer_t::vertex_description_t{{top_right.x, bottom_left.y}, {tex_range[1], 0.0}});

	model.emplace_back(writer_t::vertex_description_t{bottom_left,                  {tex_range[0], 0.0}});
	model.emplace_back(writer_t::vertex_description_t{{bottom_left.x, top_right.y}, {tex_range[0], 1.0}});
	model.emplace_back(writer_t::vertex_description_t{top_right,                    {tex_range[1], 1.0}});
}

template <class CharT>
writer_t::model_t writer_t::write(
	const std::basic_string<CharT>& string,
	float size,
	horizontal_alignment_t halign,
	vertical_alignment_t valign,
	scale_t scale
) {
	model_t result;
	glm::vec2 origin(0.0);
	float top = std::numeric_limits<float>::min();
	float bottom = std::numeric_limits<float>::max();
	for(auto c : string) {
		const auto& g = m_font[c];
		add_glyph(result, origin, g);
		origin += g.advance;
		top = std::max(top, g.bottom_left.y+g.size.y);
		bottom = std::min(bottom, g.bottom_left.y);
	}
	const auto height = top-bottom;

	const auto& first = m_font[string.front()];
	const auto& last = m_font[string.back()];
	const auto left = first.bottom_left.x;
	const auto right = origin.x-last.advance.x+last.bottom_left.x+last.size.x;
	const auto width = right - left;

	float scale_factor;
	switch(scale) {
		case scale_t::y_axis:
			scale_factor = size/height;
			break;
		case scale_t::x_axis:
			scale_factor = size/width;
			break;
	}

	glm::vec2 offset;

	switch(halign) {
		case horizontal_alignment_t::origin:
			break;
		case horizontal_alignment_t::left:
			offset.x=left;
			break;
		case horizontal_alignment_t::center:
			offset.x=left+width/2;
			break;
		case horizontal_alignment_t::right:
			offset.x=right;
			break;
	}
	switch(valign) {
		case vertical_alignment_t::bottom:
			offset.y=bottom;
			break;
		case vertical_alignment_t::baseline:
			break;
		case vertical_alignment_t::middleline:
			throw std::runtime_error("not implemented");
			break;
		case vertical_alignment_t::center:
			offset.y=bottom+height/2;
			break;
		case vertical_alignment_t::top:
			offset.y=bottom;
			break;
	}

	if(offset != glm::vec2(0.0)) {
		for(auto& vertex : result) {
			vertex.position-=offset;
			vertex.position*=scale_factor;
		}
	}

	return result;
}

template writer_t::model_t writer_t::write<char>(const std::basic_string<char>&, float, horizontal_alignment_t, vertical_alignment_t, scale_t);
template writer_t::model_t writer_t::write<wchar_t>(const std::basic_string<wchar_t>&, float, horizontal_alignment_t, vertical_alignment_t, scale_t);
template writer_t::model_t writer_t::write<char16_t>(const std::basic_string<char16_t>&, float, horizontal_alignment_t, vertical_alignment_t, scale_t);
template writer_t::model_t writer_t::write<char32_t>(const std::basic_string<char32_t>&, float, horizontal_alignment_t, vertical_alignment_t, scale_t);

}
