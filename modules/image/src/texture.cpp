#include "glpp/core/object/texture.hpp"
#include <stb_image.h>
#include <memory>

namespace glpp::core::object::detail {

stbi_image_t::stbi_image_t(const char* filename, int channels)
{
// 	stbi_set_flip_vertically_on_load(true);
	m_storage = stbi_load(
		filename,
		&m_width,
		&m_height,
		&m_channels,
		channels
	);
	m_channels = channels;
}

stbi_image_t::~stbi_image_t() {
	stbi_image_free(m_storage);
}

int stbi_image_t::width() const {
	return m_width;
}

int stbi_image_t::height() const {
	return m_height;
}

const stbi_image_t::value_type* stbi_image_t::begin() const {
	return m_storage;
}

const stbi_image_t::value_type* stbi_image_t::end() const {
	return m_storage + m_width*m_height*m_channels;
}

} // End of namespace glpp::object::detail
