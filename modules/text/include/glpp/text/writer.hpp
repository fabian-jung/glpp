#pragma once

#include "font.hpp"
#include "glpp/core/render/model.hpp"
#include "glpp/core/render/renderer.hpp"

namespace glpp::text {

enum class horizontal_alignment_t {
	left,
	origin,
	center,
	origin_right,
	right
};

enum class paragraph_alignment_t {
	left,
	center,
	right,
	justified
};

enum class vertical_alignment_t {
	bottom,
	baseline,
	middleline,
	center,
	top
};

struct label_t {
	glm::vec2 size;
	glm::vec2 origin = glm::vec2(0.0);
	horizontal_alignment_t halign = horizontal_alignment_t::origin;
	vertical_alignment_t valign = vertical_alignment_t::baseline;
};

struct text_box_t {
	glm::vec2 size;
	float text_size = 1;
	glm::vec2 origin = glm::vec2(0,0);
	horizontal_alignment_t halign = horizontal_alignment_t::origin;
	vertical_alignment_t valign = vertical_alignment_t::baseline;
	paragraph_alignment_t palign = paragraph_alignment_t::left;
};

class writer_t {
public:
	struct vertex_description_t {
		glm::vec2 position;
		glm::vec2 tex;
	};

	struct uniform_description_t {
		glm::vec4 color;
	};

	using model_t = glpp::core::render::model_t<vertex_description_t>;

	static glpp::core::object::shader_t vertex_shader();
	static glpp::core::object::shader_t fragment_shader();

	glpp::core::render::renderer_t<uniform_description_t> renderer();

	writer_t(const font_t& atlas);

	writer_t(writer_t&& mov) = default;
	writer_t& operator=(writer_t&& mov) = delete;

	writer_t(const writer_t& mov) = default;
	writer_t& operator=(const writer_t& mov) = delete;

	template <class CharT>
	model_t write(
		const label_t& description,
		const std::basic_string<CharT>& string
	);

	template <class CharT>
	model_t write(
		const text_box_t& description,
		const std::basic_string<CharT>& string
	);

private:
	const font_t& m_font;
};

}
