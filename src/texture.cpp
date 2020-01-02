#include "glpp/object/texture.hpp"
#include <algorithm>

namespace glpp::object {

texture_t::texture_t(
	const size_t& width,
	const size_t& height,
	image_format_t format,
	const clamp_mode_t& clamp_mode,
	const filter_mode_t& filter,
	const mipmap_mode_t& mipmap_mode
) :
	object_t<>(init(), destroy),
	m_width(width),
	m_height(height),
	m_format(static_cast<GLenum>(format))
{
	glpp::call(glTextureParameteri, id(), GL_TEXTURE_WRAP_S, static_cast<GLenum>(clamp_mode));
	glpp::call(glTextureParameteri, id(), GL_TEXTURE_WRAP_T, static_cast<GLenum>(clamp_mode));

	switch(mipmap_mode) {
		case mipmap_mode_t::none:
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(filter));
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(filter));
			break;
		case mipmap_mode_t::nearest:
		{
			const GLenum mode = filter==filter_mode_t::nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR;
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(mode));
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mode));
			break;
		}
		case mipmap_mode_t::linear:
		{
			const GLenum mode = filter==filter_mode_t::nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(mode));
			glpp::call(glTextureParameteri, id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mode));
			break;
		}
	}

	constexpr int total_level_of_detail = 1;
	glpp::call(glTextureStorage2D,
		id(),
		total_level_of_detail,
		static_cast<GLenum>(format),
		width,
		height
	);
}

texture_slot_t texture_t::bind_to_texture_slot() const {
	return texture_slot_t(*this);
}

GLuint texture_t::init() {
	GLuint tex;
	glpp::call(glCreateTextures, GL_TEXTURE_2D, 1, &tex);
	return tex;
}

void texture_t::destroy(GLuint id) {
	glpp::call(glDeleteTextures, 1, &id);
}

size_t texture_t::width() const {
	return m_width;
}

size_t texture_t::height() const {
	return m_height;
}

texture_slot_t::texture_slot_t(const texture_t& texture) :
	m_id(next_free_id())
{
	if(id() >= max_texture_units()) {
		throw std::runtime_error("Trying to bind texture unit to an invalid offset.");
	}
	units[id()] = texture.id();
	glpp::call(glBindTextureUnit, id(), texture.id());
}

texture_slot_t::texture_slot_t(texture_slot_t&& mov) = default;
texture_slot_t& texture_slot_t::operator=(texture_slot_t&& mov) = default;

texture_slot_t::~texture_slot_t() {
	units[id()] = -1;
}

GLint texture_slot_t::id() const {
	return m_id;
}

int texture_slot_t::next_free_id() {
	auto it = std::find(units.begin(), units.end(), -1);
	if(it == units.end()) {
		units.emplace_back(-1);
		it = units.end()-1;
	}
	return it - units.begin();
}

int max_texture_units_impl() {
	int i;
	glpp::call(glGetIntegerv, GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
	return i;
}

int texture_slot_t::max_texture_units() const {
	static const int units = max_texture_units_impl();
	return units;
}

std::vector<int> texture_slot_t::units;

} // End of namespace glpp::object
