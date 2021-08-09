#include "glpp/core/object/texture.hpp"
#include <algorithm>

namespace glpp::core::object {

texture_t::texture_t(
	const size_t width,
	const size_t height,
	image_format_t format,
	const clamp_mode_t clamp_mode,
	const filter_mode_t filter,
	const mipmap_mode_t mipmap_mode,
	std::array<texture_channel_t, 4> swizzle_mask
) :
	object_t<>(init(), destroy),
	m_width(width),
	m_height(height),
	m_format(static_cast<GLenum>(format))
{
	if(format == image_format_t::preferred) {
		throw std::runtime_error("image_format_t::preferred can not be used in this overload of the constructor.");
	}

	glTextureParameteri(id(), GL_TEXTURE_WRAP_S, static_cast<GLenum>(clamp_mode));
	glTextureParameteri(id(), GL_TEXTURE_WRAP_T, static_cast<GLenum>(clamp_mode));

	switch(mipmap_mode) {
		case mipmap_mode_t::none:
			glTextureParameteri(id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(filter));
			glTextureParameteri(id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(filter));
			break;
		case mipmap_mode_t::nearest:
		{
			const GLenum mode = filter==filter_mode_t::nearest ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_LINEAR;
			glTextureParameteri(id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(mode));
			glTextureParameteri(id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mode));
			break;
		}
		case mipmap_mode_t::linear:
		{
			const GLenum mode = filter==filter_mode_t::nearest ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR_MIPMAP_LINEAR;
			glTextureParameteri(id(), GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(mode));
			glTextureParameteri(id(), GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mode));
			break;
		}
	}

	constexpr std::array<texture_channel_t, 4> default_swizzle {
		texture_channel_t::red,
		texture_channel_t::green,
		texture_channel_t::blue,
		texture_channel_t::alpha
	};

	if(swizzle_mask != default_swizzle) {
		glTextureParameteriv(id(), GL_TEXTURE_SWIZZLE_RGBA, reinterpret_cast<const GLint*>(swizzle_mask.data()));
	}

	constexpr int total_level_of_detail = 1;
	glTextureStorage2D(
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
	glCreateTextures(GL_TEXTURE_2D, 1, &tex);
	return tex;
}

void texture_t::destroy(GLuint id) {
	glDeleteTextures(1, &id);
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
	units[id()] = std::numeric_limits<int>::max();
}

texture_slot_t::texture_slot_t(const texture_t& texture) :
	m_id(next_free_id())
{
	if(id() >= max_texture_units()) {
		throw std::runtime_error("Trying to bind texture unit to an invalid offset.");
	}
	units[id()] = texture.id();
	glBindTextureUnit(id(), texture.id());
}

texture_slot_t::texture_slot_t(texture_slot_t&& mov) :
	m_id(mov.m_id)
{
	mov.m_id = -1;
}

texture_slot_t& texture_slot_t::operator=(texture_slot_t&& mov) {
	m_id = mov.m_id;
	mov.m_id = -1;
	return *this;
}

texture_slot_t::~texture_slot_t() {
	if(m_id >= 0) {
		units[id()] = -1;
	}
}

GLint texture_slot_t::id() const {
	assert(m_id >= 0);
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
	glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &i);
	return i;
}

int texture_slot_t::max_texture_units() const {
	static const int units = max_texture_units_impl();
	return units;
}

std::vector<int> texture_slot_t::units;

} // End of namespace glpp::object
