#include "glpp/text/writer.hpp"

#include "glpp/text/util/iterator.hpp"
#include <algorithm>

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

glpp::render::renderer_t<writer_t::uniform_description_t> writer_t::renderer() {
	glpp::render::renderer_t<uniform_description_t> result {
		vertex_shader(),
		fragment_shader()
	};
	result.set_texture("glyphs", m_font.texture_slot());
	result.set_uniform_name(&uniform_description_t::color, "color");
	return result;
}

glm::vec2 add_glyph(writer_t::model_t& model, const glm::vec2& origin, const glyph_t& glyph, float scale = 1.0f) {
	const auto& bottom_left = origin+glyph.bottom_left*scale;
	const auto top_right = bottom_left+glyph.size*scale;
	const auto& tex_range = glyph.tex_range;
	model.emplace_back(writer_t::vertex_description_t{bottom_left,                  {tex_range[0], 0.0}});
	model.emplace_back(writer_t::vertex_description_t{top_right,                    {tex_range[1], 1.0}});
	model.emplace_back(writer_t::vertex_description_t{{top_right.x, bottom_left.y}, {tex_range[1], 0.0}});

	model.emplace_back(writer_t::vertex_description_t{bottom_left,                  {tex_range[0], 0.0}});
	model.emplace_back(writer_t::vertex_description_t{{bottom_left.x, top_right.y}, {tex_range[0], 1.0}});
	model.emplace_back(writer_t::vertex_description_t{top_right,                    {tex_range[1], 1.0}});
	return origin+glyph.advance*scale;
}

template <class forward_it>
void align(
	const forward_it& begin,
	const forward_it& end,
	const vertical_alignment_t& valign,
	const horizontal_alignment_t& halign,
	const glm::vec2& bottom_left,
	const glm::vec2& top_right,
	glm::vec2 offset
) {
	auto size = top_right-bottom_left;

	switch(halign) {
		case horizontal_alignment_t::origin:
			break;
		case horizontal_alignment_t::left:
			offset.x-=bottom_left.x;
			break;
		case horizontal_alignment_t::center:
			offset.x-=bottom_left.x+size.x/2;
			break;
		case horizontal_alignment_t::origin_right:
			offset.x-=offset.x;
			break;
		case horizontal_alignment_t::right:
			offset.x-=top_right.x;
			break;
	}
	switch(valign) {
		case vertical_alignment_t::bottom:
			offset.y-=bottom_left.y;
			break;
		case vertical_alignment_t::baseline:
			break;
		case vertical_alignment_t::middleline:
			throw std::runtime_error("not implemented");
			break;
		case vertical_alignment_t::center:
			offset.y-=bottom_left.y+size.y/2;
			break;
		case vertical_alignment_t::top:
			offset.y-=top_right.y;
			break;
	}

	if(offset != glm::vec2(0.0)) {
		std::for_each(begin, end, [offset](auto& vertex){
			vertex.position+=offset;
		});
	}
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
		origin = add_glyph(result, origin, g);
		top = std::max(top, g.bottom_left.y+g.size.y);
		bottom = std::min(bottom, g.bottom_left.y);
	}
	const auto height = top-bottom;

	const auto& first = m_font[string.front()];
	const auto& last = m_font[string.back()];
	const auto left = first.bottom_left.x;
	const auto right = origin.x-last.advance.x+last.bottom_left.x+last.size.x;
	const auto width = right - left;

	auto goal_scale = glm::vec2{
		description.size.x == 0 ? std::numeric_limits<float>::max() : description.size.x,
		description.size.y == 0 ? std::numeric_limits<float>::max() : description.size.y
	};
	glm::vec2 scale_factor = goal_scale/glm::vec2(width, height);
	const auto scale = std::min(scale_factor.x, scale_factor.y);

	for(auto& vertex : result) {
		vertex.position*=scale;
	}

	align(result.begin(), result.end(), description.valign, description.halign, scale*glm::vec2{left, bottom}, scale*glm::vec2{right, top}, description.origin);

	return result;
}

template writer_t::model_t writer_t::write<char>(const label_t&, const std::basic_string<char>&);
template writer_t::model_t writer_t::write<wchar_t>(const label_t&, const std::basic_string<wchar_t>&);
template writer_t::model_t writer_t::write<char16_t>(const label_t&, const std::basic_string<char16_t>&);
template writer_t::model_t writer_t::write<char32_t>(const label_t&, const std::basic_string<char32_t>&);

class word_t {
public:
	template <class CharT>
	word_t(const std::basic_string_view<CharT>& string, const font_t& font, float size)
	{
		glm::vec2 advance { 0.0 };
		std::transform(
			string.begin(),
			string.end(),
			std::back_inserter(m_glyphs),
			[&](auto c){
				auto glyph = font[c];
				top_right = glm::max(top_right, advance+size*(glyph.bottom_left+glyph.size));
				bottom_left = glm::min(bottom_left, advance+size*glyph.bottom_left);
				advance += glyph.advance*size;
				return glyph;
			});
	}

	auto begin() {
		return m_glyphs.begin();
	}

	auto begin() const {
		return m_glyphs.begin();
	}

	auto end() {
		return m_glyphs.end();
	}

	auto end() const {
		return m_glyphs.end();
	}

	glm::vec2 top_right { std::numeric_limits<float>::min() };
	glm::vec2 bottom_left { std::numeric_limits<float>::max() };
private:
	std::vector<glyph_t> m_glyphs;
};

glm::vec2 add_word(writer_t::model_t& model, glm::vec2 origin, const word_t& word, float scale = 1.0f) {
	for(const auto& glyph : word) {
		origin = add_glyph(model, origin, glyph, scale);
	}
	return origin;
}

template <class forward_it>
void align(
	const forward_it& begin,
	const forward_it& end,
	const paragraph_alignment_t& palign,
	const float& left,
	const float& right,
	const float& fill
) {
	float offset = 0;
	switch(palign) {
		case paragraph_alignment_t::left:
			break;
		case paragraph_alignment_t::center:
			offset=(fill-right)/2;
			break;
		case paragraph_alignment_t::right:
			offset=fill-right;
			break;
		case paragraph_alignment_t::justified:
			throw std::runtime_error("not implemented");
			break;
	}
	std::for_each(begin, end, [offset](auto& vertex){
		vertex.position.x+=offset;
	});
}

template <class CharT>
writer_t::model_t writer_t::write(
	const text_box_t& description,
	const std::basic_string<CharT>& string
) {
	model_t result;
	glm::vec2 origin {0.0};
	glm::vec2 top_right { std::numeric_limits<float>::min() };
	glm::vec2 bottom_left { std::numeric_limits<float>::max() };
	float space = m_font[' '].advance.x*description.text_size;
	origin.y = description.text_size;
	for(auto paragraph : util::string_splitter_t(string, static_cast<CharT>('\n'))) {
		origin.x = -space;
		origin.y -= description.text_size;
		float line_left = std::numeric_limits<float>::max();
		float line_right = std::numeric_limits<float>::min();
		auto line_begin = result.size();
		for(auto word : util::string_splitter_t(paragraph, static_cast<CharT>(' '))) {
			word_t w { word, m_font, description.text_size };
			do {
				origin.x += space;
				if(origin.x+w.top_right.x <= description.size.x) {
					top_right =   glm::max(top_right,    origin  +w.top_right);
					bottom_left = glm::min(bottom_left,  origin  +w.bottom_left);
					line_right =  std::max(line_right,   origin.x+w.top_right.x);
					line_left =   std::min(line_left,    origin.x+w.bottom_left.x);
					origin = add_word(result, origin, w, description.text_size);
					word = word.substr(0,0);
				} else {
					if(origin.x == 0) {
						throw std::runtime_error("Overflow textbox. Add manual spaces or linebreaks");
					}
					const auto line_end = result.size();
					align(result.begin()+line_begin, result.begin()+line_end, description.palign, line_left, line_right, description.size.x);
					line_begin = result.size();
					origin.x = -space;
					origin.y -= description.text_size;
					line_left = std::numeric_limits<float>::max();
					line_right = std::numeric_limits<float>::min();
				}
			} while(!word.empty());
		}
		const auto line_end = result.size();
		align(result.begin()+line_begin, result.begin()+line_end, description.palign, line_left, line_right, description.size.x);
	}
	align(result.begin(), result.end(), description.valign, description.halign, bottom_left, top_right, description.origin);
	return result;
}

template writer_t::model_t writer_t::write<char>(const text_box_t&, const std::basic_string<char>&);
template writer_t::model_t writer_t::write<wchar_t>(const text_box_t&, const std::basic_string<wchar_t>&);
template writer_t::model_t writer_t::write<char16_t>(const text_box_t&, const std::basic_string<char16_t>&);
template writer_t::model_t writer_t::write<char32_t>(const text_box_t&, const std::basic_string<char32_t>&);

}
