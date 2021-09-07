#include "glpp/core/object/image.hpp"
#include <stb_image.h>
#include <stb_image_write.h>
#include <memory>
#include <cstring>
#include <string_view>

namespace glpp::core::object {
namespace detail {

stbi_image_t::stbi_image_t(const char* filename, int channels)
{
	m_storage = stbi_load(
		filename,
		&m_width,
		&m_height,
		&m_channels,
		channels
	);
	if(m_storage == nullptr) {
		throw std::runtime_error(std::string("Image ")+filename+" could not be found or opened.");
	} 
	m_channels = channels;
}

stbi_image_t::stbi_image_t(size_t width, size_t height, int channels) :
	m_width(width),
	m_height(height),
	m_channels(channels),
	m_storage(
		reinterpret_cast<value_type*>(
			malloc(width*height*channels*sizeof(value_type))
		)
	)
{}

stbi_image_t::~stbi_image_t() {
	stbi_image_free(m_storage);
}

int png_write_adapter(const char* filename, int w, int h, int comp, const void* data) {
	return stbi_write_png(filename, w, h , comp, data, w*comp*sizeof(stbi_image_t::value_type));
}

int jpg_write_adapter(const char* filename, int w, int h, int comp, const void* data) {
	return stbi_write_jpg(filename, w, h , comp, data, 99);
}

void stbi_image_t::write(const char* filename) const {
	using write_function_t = int (*)(const char*, int, int, int, const void*);
	using write_function_map_t = std::unordered_map<std::string_view, write_function_t>;

	static const write_function_map_t write_functions {
		{ "bmp", stbi_write_bmp },
		{ "png", png_write_adapter },
		{ "tga", stbi_write_tga },
		{ "jpg", jpg_write_adapter },
		{ "jpeg", jpg_write_adapter }
	};

	const std::string_view filename_sv = filename;
	const auto dot_pos = filename_sv.find_last_of('.');
	if(dot_pos != filename_sv.npos) {
		const auto extention = filename_sv.substr(dot_pos+1);
		const auto kv = write_functions.find(extention);
		if(kv == write_functions.end()) {
			throw std::runtime_error("Images can not be saved with the provided file extention.");
		}
		kv->second(filename, m_width, m_height, m_channels, m_storage);
	} else {
		throw std::runtime_error("Filename has no extention provided.");
	}
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

stbi_image_t::value_type* stbi_image_t::begin() {
	return m_storage;
}

stbi_image_t::value_type* stbi_image_t::end() {
	return m_storage + m_width*m_height*m_channels;
}

} // End of namespace glpp::object::detail

}