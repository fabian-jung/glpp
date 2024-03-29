#pragma once

#include "charset.hpp"
#include <glm/glm.hpp>
#include "glpp/core/object/texture.hpp"
#include <memory>

namespace glpp::text {

struct glyph_t {
	glm::vec2 bottom_left;
	glm::vec2 size;
	glm::vec2 advance;
	glm::vec2 tex_range;
};


class font_t {
public:
	using char_t = charset_t::char_t;

	font_t(const std::string& path_to_ttf, unsigned int vertical_resolution, charset_t charset = charset_t::ascii());
	~font_t();

	font_t(font_t&& mov);
	font_t& operator=(font_t&& mov);

	font_t(const font_t& cpy) = delete;
	font_t& operator=(const font_t& cpy) = delete;

	const glyph_t& operator[](char_t c) const;
	const glpp::core::object::texture_slot_t& texture_slot() const;

private:
	auto init_glyphs(charset_t& charset);
	glpp::core::object::texture_t init_texture() const;

	class font_loader_t;
	std::unique_ptr<font_loader_t> m_loader;
	glm::vec2 m_atlas_size;
	/*const*/ std::unordered_map<char_t, glyph_t> m_glyphs;
	/*const*/ glpp::core::object::texture_t m_texture_atlas;
	glpp::core::object::texture_slot_t m_texture_slot;
};

}
