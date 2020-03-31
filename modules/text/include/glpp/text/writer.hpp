#pragma once

#include "font.hpp"
#include "glpp/render/model.hpp"
#include "glpp/render/renderer.hpp"

namespace glpp::text {

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

	enum class scale_t {
		y_axis,
		x_axis
	};

	writer_t(font_t&& atlas);

	template <class CharT>
	model_t write(
		const std::basic_string<CharT>& string,
		float size,
		horizontal_alignment_t halign = horizontal_alignment_t::origin,
		vertical_alignment_t valign = vertical_alignment_t::baseline,
		scale_t scale = scale_t::y_axis
	);

private:
	const font_t m_font;
};

}
