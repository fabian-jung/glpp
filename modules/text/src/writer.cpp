#include "glpp/text/writer.hpp"

namespace glpp::text {

writer_t::writer_t(const font_t& atlas) :
	m_font(atlas)
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
	const label_t& description,
	const std::basic_string<CharT>& string
) {
	glm::vec2 origin(0.0);
	model_t result;
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

	glm::vec2 scale_factor = description.size/glm::vec2(width, height);

	glm::vec2 offset(0.0);
	switch(description.halign) {
		case horizontal_alignment_t::origin:
			break;
		case horizontal_alignment_t::left:
			offset.x=-left;
			break;
		case horizontal_alignment_t::center:
			offset.x=-left-width/2;
			break;
		case horizontal_alignment_t::right:
			offset.x=-right;
			break;
	}
	switch(description.valign) {
		case vertical_alignment_t::bottom:
			offset.y=-bottom;
			break;
		case vertical_alignment_t::baseline:
			break;
		case vertical_alignment_t::middleline:
			throw std::runtime_error("not implemented");
			break;
		case vertical_alignment_t::center:
			offset.y=-bottom-height/2;
			break;
		case vertical_alignment_t::top:
			offset.y=-top;
			break;
	}

	if(offset != glm::vec2(0.0)) {
		for(auto& vertex : result) {
			vertex.position+=offset;
			vertex.position*=std::max(scale_factor.x, scale_factor.y);
			vertex.position+=description.origin;
		}
	}

	return result;
}

template writer_t::model_t writer_t::write<char>(const label_t&, const std::basic_string<char>&);
template writer_t::model_t writer_t::write<wchar_t>(const label_t&, const std::basic_string<wchar_t>&);
template writer_t::model_t writer_t::write<char16_t>(const label_t&, const std::basic_string<char16_t>&);
template writer_t::model_t writer_t::write<char32_t>(const label_t&, const std::basic_string<char32_t>&);

// template <class string_it>
// class line_t {
// public:
// 	line_t() = default;
//
// 	line_t(string_it begin, string_it end, const font_t& font, float line_width) {
// 		float last_space_width = 0;
// 		auto last_space = begin;
// 		auto line_end = begin;
// 		for(; line_end != end; ++line_end) {
// 			const auto c = *line_end;
// 			if(c == '\n') continue;
// 			if(c == ' ') {
// 				last_space = line_end;
// 				last_space_width = m_width;
// 			}
// 			m_width += font[c].advance.x;
// 			if(m_width >= line_width) {
// 				line_end = last_space;
// 				m_width = last_space_width;
// 				continue;
// 			}
// 		}
// 		m_words.emplace_back(begin);
// 		while((begin = std::find(begin, end, ' ')) != end) {
// 			m_words.emplace_back(begin+1);
// 		}
// 		m_words.emplace_back(begin);
// 	}
//
// 	size_t word_count() {
// 		return m_words.size()-1;
// 	}
//
// 	auto begin() {
// 		return m_words.begin();
// 	}
//
// 	auto end() {
// 		return m_words.end()-1;
// 	}
//
// 	float width();
//
// private:
// 	float m_width = 0;
// 	std::vector<string_it> m_words;
// };
//
// template <class forward_it>
// static bool split_line(forward_it& begin, forward_it end, const font_t font, float line_width, line_t<forward_it>& line) {
// 	if(begin == end) return false;
// 	line = line_t(begin, end, font, line_width);
// 	begin = *line.end();
// 	return true;
// }
//
//
// template <class CharT>
// writer_t::model_t writer_t::flow_hbox(
// 	const std::basic_string<CharT>& string,
// 	float vertical_size,
// 	float width,
// 	paragraph_alignment_t palign,
// 	horizontal_alignment_t halign,
// 	vertical_alignment_t valign
// ) {
// 	model_t result;
// 	auto pos = string.begin();
// 	line_t<typename std::basic_string<CharT>::iterator> line;
// 	glm::vec2 origin(0.0);
// 	while (split_line(pos, string.end(), line)) {
// 		auto diff = width - line.width();
// 		auto offset = palign == paragraph_alignment_t::right ? diff : 0;
// 		const auto joffset =  (diff - offset)/(line.word_count()-1);
// 		for(auto word : line.words()) {
// 			add_word(result, origin, word);
// 			origin.x += joffset;
// 		}
// 		origin.x = 0;
// 		origin.y += vertical_size;
// 	}
//
// 	return result;
// }
//
// template writer_t::model_t writer_t::flow_hbox<char>(const std::basic_string<char>&, float, float, paragraph_alignment_t, horizontal_alignment_t, vertical_alignment_t);
// template writer_t::model_t writer_t::flow_hbox<wchar_t>(const std::basic_string<wchar_t>&, float, float, paragraph_alignment_t, horizontal_alignment_t, vertical_alignment_t);
// template writer_t::model_t writer_t::flow_hbox<char16_t>(const std::basic_string<char16_t>&, float, float, paragraph_alignment_t, horizontal_alignment_t, vertical_alignment_t);
// template writer_t::model_t writer_t::flow_hbox<char32_t>(const std::basic_string<char32_t>&, float, float, paragraph_alignment_t, horizontal_alignment_t, vertical_alignment_t);

}
