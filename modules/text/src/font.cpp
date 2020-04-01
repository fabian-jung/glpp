#include "glpp/text/font.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glpp/core.hpp>
#include <memory>

namespace glpp::text {

class font_loader_t {
public:
	font_loader_t(const std::string& path_to_ttf, unsigned int vertical_resolution);
	~font_loader_t();

	auto get_char(font_t::char_t c);

private:
	FT_Library m_ft;
	FT_Face m_face;
};

font_loader_t::font_loader_t(const std::string& path_to_ttf, unsigned int vertical_resolution) {
	if(FT_Init_FreeType(&m_ft)) {
		throw std::runtime_error("Could not initialise freetype library.");
	}

	if(FT_New_Face(m_ft, path_to_ttf.c_str(), 0, &m_face)) {
		throw std::runtime_error("Failed to load font.");
	}

	FT_Set_Pixel_Sizes(m_face, 0, vertical_resolution);
}

auto font_loader_t::get_char(font_t::char_t c) {
	if(FT_Load_Char(m_face, c, FT_LOAD_RENDER)) {
		throw std::runtime_error("Failed to load Glyph");
	}

	auto& g = m_face->glyph;

	std::pair<glyph_t, std::uint8_t*> result;
	auto& [glyph, pixbuf] = result;
	glyph.advance.x = static_cast<float>(g->advance.x)/64.0f;
	glyph.advance.y = static_cast<float>(g->advance.y)/64.0f;
	glyph.size.x = g->bitmap.width;
	glyph.size.y = g->bitmap.rows;
	glyph.bottom_left.x = g->bitmap_left;
	glyph.bottom_left.y = g->bitmap_top-glyph.size.y;
	pixbuf = g->bitmap.buffer;

	return result;
}

font_loader_t::~font_loader_t() {
	FT_Done_Face(m_face);
	FT_Done_FreeType(m_ft);
}

font_t::~font_t() = default;
font_t::font_t(font_t&& mov) = default;
font_t& font_t::operator=(font_t&& mov) = default;

auto font_t::init_glyphs(charset_t& charset) {
	std::remove_const_t<decltype(m_glyphs)> result;
	charset.populate();

	for(auto c : charset) {
		result[c] = m_loader->get_char(c).first;
		m_atlas_size.x += result[c].size.x;
	}

	float begin = 0;
	for(auto& [c, glyph] : result) {
		const auto step = glyph.size.x/m_atlas_size.x;
		glyph.tex_range[0] = begin;
		glyph.tex_range[1] = begin+step;
		begin = glyph.tex_range[1] + 1.0/m_atlas_size.x;
	}

	return result;
}

glpp::object::texture_t font_t::init_texture() const {

	glpp::object::texture_t result {
		glpp::object::image_t<std::uint8_t>(m_atlas_size.x, m_atlas_size.y),
		object::image_format_t::prefered,
		object::clamp_mode_t::clamp_to_edge
	};
	glpp::object::framebuffer_t fb({{result, glpp::object::attachment_t::color}});
	fb.bind(glpp::object::framebuffer_target_t::write);

	struct vertex_description_t {
		glm::vec2 pos;
	};
	using model_t = glpp::render::model_t<vertex_description_t>;

	glpp::render::view_t view{
		model_t {
			vertex_description_t{{0.0, 0.0}},
			vertex_description_t{{1.0, 1.0}},
			vertex_description_t{{1.0, 0.0}},

			vertex_description_t{{0.0, 0.0}},
			vertex_description_t{{0.0, 1.0}},
			vertex_description_t{{1.0, 1.0}}
		},
		&vertex_description_t::pos
	};

	struct uniform_description_t {
		float left, right;
	};
	glpp::render::renderer_t<uniform_description_t> renderer{
		glpp::object::shader_t(glpp::object::shader_type_t::vertex,
			"#version 330 core\n\
			layout (location = 0) in vec2 pos;\n\
			out vec2 tex;\n\
			uniform float left;\n\
			uniform float right;\n\
			\n\
			void main()\n\
			{\n\
				tex = vec2(pos.x, 1-pos.y);\n\
				gl_Position = vec4(2*(left+pos.x*(right-left))-1.0, 2*pos.y-1.0, 0.0, 1.0);\n\
			}"
		),
		glpp::object::shader_t(glpp::object::shader_type_t::fragment,
			"#version 330 core\n\
			in vec2 tex;\n\
			uniform sampler2D glyph;\n\
			out vec4 FragColor;\n\
			\n\
			void main()\n\
			{\n\
				FragColor = texture(glyph, tex);\n\
			}"
		)
	};
	renderer.set_uniform_name( &uniform_description_t::left, "left");
	renderer.set_uniform_name( &uniform_description_t::right, "right");
	glpp::call(glClearColor, 0.0, 0.0, 0.0, 0.0);
	glpp::call(glClear, GL_COLOR_BUFFER_BIT);
	glpp::call(glViewport, 0, 0, m_atlas_size.x, m_atlas_size.y);
	for(const auto& [c, glyph] : m_glyphs) {
		if(glyph.size.x > 0) {
			renderer.set_uniform(&uniform_description_t::left, glyph.tex_range[0]);
			renderer.set_uniform(&uniform_description_t::right, glyph.tex_range[1]);
			const auto& [_, pixbuf] = m_loader->get_char(c);
			glpp::object::texture_t glyph_texture {
				glpp::object::image_t<std::uint8_t>{
					static_cast<size_t>(glyph.size.x),
					static_cast<size_t>(glyph.size.y),
					pixbuf
				},
				object::image_format_t::prefered,
				object::clamp_mode_t::clamp_to_edge
			};
			auto slot = glyph_texture.bind_to_texture_slot();
			renderer.set_texture("glyph", slot);
			renderer.render(view);
		}
	}
	glpp::object::framebuffer_t::bind_default_framebuffer(glpp::object::framebuffer_target_t::write);
	return result;
}


font_t::font_t(const std::string& path_to_ttf, unsigned int vertical_resolution, charset_t charset) :
	m_loader(std::make_unique<font_loader_t>(path_to_ttf, vertical_resolution)),
	m_atlas_size(0.0, static_cast<float>(vertical_resolution)),
	m_glyphs(init_glyphs(charset)),
	m_texture_atlas(init_texture()),
	m_texture_slot(m_texture_atlas.bind_to_texture_slot())
{
	m_loader.reset();
}

const glyph_t& font_t::operator[](char_t c) const {
	return m_glyphs.at(c);
}

const glpp::object::texture_slot_t& font_t::texture_slot() const {
	return m_texture_slot;
}


}
