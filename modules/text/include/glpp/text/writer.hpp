#pragma once

#include "font.hpp"
#include "glpp/render/model.hpp"
#include "glpp/render/renderer.hpp"

namespace glpp::text {

enum class horizontal_alignment_t {
	origin,
	left,
	center,
	right
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

};

class writer_t {
public:
	using vertex_description_t = struct vertex_description_t {
		glm::vec2 position;
		glm::vec2 tex;
	};

	using model_t = glpp::render::model_t<vertex_description_t>;

	static glpp::object::shader_t vertex_shader();
	static glpp::object::shader_t fragment_shader();

	glpp::render::renderer_t<> renderer();

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

private:
	const font_t& m_font;
};

}
