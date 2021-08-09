#pragma once

#include "glpp/gl.hpp"
#include "glpp/core/object.hpp"
#include "glpp/core/object/attribute_properties.hpp"
#include "glpp/core/object/image.hpp"
#include <vector>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <limits>

namespace glpp::core::object {

enum class clamp_mode_t : GLenum  {
	repeat = GL_REPEAT,
	mirrored_repeat = GL_MIRRORED_REPEAT,
	clamp_to_edge = GL_CLAMP_TO_EDGE,
	clamp_to_border = GL_CLAMP_TO_BORDER
};

enum class filter_mode_t : GLenum {
	nearest = GL_NEAREST,
	linear = GL_LINEAR
};

enum class mipmap_mode_t {
	none,
	nearest,
	linear
};

class texture_slot_t;

enum class texture_channel_t : GLenum {
	red = GL_RED,
	green = GL_GREEN,
	blue = GL_BLUE,
	alpha = GL_ALPHA,
	zero = GL_ZERO,
	one = GL_ONE
};

class texture_t : public object_t<> {
public:

	texture_t(texture_t&& mov) = default;
	texture_t& operator=(texture_t&& mov) = default;

	texture_t(const texture_t& cpy) = delete;
	texture_t& operator=(const texture_t& cpy) = delete;

	texture_t(
		const size_t width,
		const size_t height,
		image_format_t format = image_format_t::rgb_8,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		std::array<texture_channel_t, 4> swizzle_mask = {texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}
	);

	template <class T>
	texture_t(
		const image_t<T>& image,
		image_format_t format = image_format_t::preferred,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		std::array<texture_channel_t, 4> swizzle_mask = {texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}
	);

	texture_slot_t bind_to_texture_slot() const;

	template <class T>
	void update(
		const T* pixels,
		image_format_t format = image_format_t::preferred
	);

	template <class T>
	void update(
		size_t xoffset,
		size_t yoffset,
		size_t width,
		size_t height,
		const T* pixels,
		image_format_t format = image_format_t::preferred
	);

	template <class T>
	void update(const image_t<T>& image);

	template <class T>
	void update(
		const image_t<T>& image,
		size_t xoffset,
		size_t yoffset,
		size_t width,
		size_t height
	);

	size_t width() const;
	size_t height() const;

private:
	static GLuint init();
	static void destroy(GLuint id);
	size_t m_width;
	size_t m_height;
	GLenum m_format;
};

class texture_slot_t {
public:

	texture_slot_t();
	texture_slot_t(const texture_t& texture);

	texture_slot_t(texture_slot_t&& mov);
	texture_slot_t& operator=(texture_slot_t&& mov);

	texture_slot_t(const texture_slot_t& cpy) = delete;
	texture_slot_t& operator=(const texture_slot_t& cpy) = delete;

	~texture_slot_t();

	GLint id() const;

private:
	int next_free_id();
	int max_texture_units() const;

	static std::vector<int> units;
	GLint m_id;
};

namespace detail {
template <class T>
constexpr auto resolve_format(image_format_t format, const image_t<T>& image) {
	if(format == image_format_t::preferred) {
		switch(image.channels()) {
			case 1:
				return image_format_t::red_8;
			case 2:
				return image_format_t::rg_8;
			case 3:
				return image_format_t::rgb_8;
			case 4:
				return image_format_t::rgba_8;
		}
	}
	return format;
}
}

template <class T>
texture_t::texture_t(
	const image_t<T>& image,
	image_format_t format,
	const clamp_mode_t clamp_mode,
	const filter_mode_t filter,
	const mipmap_mode_t mipmap_mode,
	std::array<texture_channel_t, 4> swizzle_mask
) :
	texture_t(image.width(), image.height(), detail::resolve_format(format, image), clamp_mode, filter, mipmap_mode, swizzle_mask)
{
	update(image);

	if(mipmap_mode != mipmap_mode_t::none) {
		glGenerateTextureMipmap(id());
	}
}

template <class T>
void texture_t::update(
	const T* pixels,
	image_format_t format
) {
	update(0,0, m_width, m_height, pixels, format);
}

template<class T>
void texture_t::update(
	size_t xoffset,
	size_t yoffset,
	size_t width,
	size_t height,
	const T* pixels,
	image_format_t format
) {
	auto base_internal_format = [](GLenum format) {
		switch(format) {
			case GL_R8                 : return GL_RED;
			case GL_R8_SNORM           : return GL_RED;
			case GL_R16                : return GL_RED;
			case GL_R16_SNORM          : return GL_RED;
			case GL_RG8                : return GL_RG;
			case GL_RG8_SNORM          : return GL_RG;
			case GL_RG16               : return GL_RG;
			case GL_RG16_SNORM         : return GL_RG;
			case GL_R3_G3_B2           : return GL_RGB;
			case GL_RGB4               : return GL_RGB;
			case GL_RGB5               : return GL_RGB;
			case GL_RGB8               : return GL_RGB;
			case GL_RGB8_SNORM         : return GL_RGB;
			case GL_RGB10              : return GL_RGB;
			case GL_RGB12              : return GL_RGB;
			case GL_RGB16_SNORM        : return GL_RGB;
			case GL_RGBA2              : return GL_RGB;
			case GL_RGBA4              : return GL_RGB;
			case GL_RGB5_A1            : return GL_RGBA;
			case GL_RGBA8              : return GL_RGBA;
			case GL_RGBA8_SNORM        : return GL_RGBA;
			case GL_RGB10_A2           : return GL_RGBA;
			case GL_RGB10_A2UI         : return GL_RGBA;
			case GL_RGBA12             : return GL_RGBA;
			case GL_RGBA16             : return GL_RGBA;
			case GL_SRGB8              : return GL_RGB;
			case GL_SRGB8_ALPHA8       : return GL_RGBA;
			case GL_R16F               : return GL_RED;
			case GL_RG16F              : return GL_RG;
			case GL_RGB16F             : return GL_RGB;
			case GL_RGBA16F            : return GL_RGBA;
			case GL_R32F               : return GL_RED;
			case GL_RG32F              : return GL_RG;
			case GL_RGB32F             : return GL_RGB;
			case GL_RGBA32F            : return GL_RGBA;
			case GL_R11F_G11F_B10F     : return GL_RGB;
			case GL_RGB9_E5            : return GL_RGB;
			case GL_R8I                : return GL_RED;
			case GL_R8UI               : return GL_RED;
			case GL_R16I               : return GL_RED;
			case GL_R16UI              : return GL_RED;
			case GL_R32I               : return GL_RED;
			case GL_R32UI              : return GL_RED;
			case GL_RG8I               : return GL_RG;
			case GL_RG8UI              : return GL_RG;
			case GL_RG16I              : return GL_RG;
			case GL_RG16UI             : return GL_RG;
			case GL_RG32I              : return GL_RG;
			case GL_RG32UI             : return GL_RG;
			case GL_RGB8I              : return GL_RGB;
			case GL_RGB8UI             : return GL_RGB;
			case GL_RGB16I             : return GL_RGB;
			case GL_RGB16UI            : return GL_RGB;
			case GL_RGB32I             : return GL_RGB;
			case GL_RGB32UI            : return GL_RGB;
			case GL_RGBA8I             : return GL_RGBA;
			case GL_RGBA8UI            : return GL_RGBA;
			case GL_RGBA16I            : return GL_RGBA;
			case GL_RGBA16UI           : return GL_RGBA;
			case GL_RGBA32I            : return GL_RGBA;
			case GL_RGBA32UI           : return GL_RGBA;
			case GL_DEPTH_COMPONENT24  : return GL_DEPTH_COMPONENT;
			case GL_DEPTH_COMPONENT16  : return GL_DEPTH_COMPONENT;
			case GL_DEPTH_COMPONENT32F : return GL_DEPTH_COMPONENT;
			case GL_DEPTH32F_STENCIL8  : return GL_DEPTH_STENCIL;
			case GL_DEPTH24_STENCIL8   : return GL_DEPTH_STENCIL;
			case GL_STENCIL_INDEX8	   : return GL_STENCIL_INDEX;
		}
		throw std::runtime_error("Unable to convert sized internal format to base internal format.");
	};

	if(format == image_format_t::preferred) format = static_cast<image_format_t>(base_internal_format(m_format));
	constexpr int level_of_detail = 0;
	glTextureSubImage2D(
		id(),
		level_of_detail,
		xoffset,
		yoffset,
		width,
		height,
		static_cast<GLenum>(format),
		attribute_properties<T>::type,
		pixels
	);
}

template <class T>
void texture_t::update(
	const image_t<T>& image,
	size_t xoffset,
	size_t yoffset,
	size_t width,
	size_t height
) {
	update(
		xoffset,
		yoffset,
		width,
		height,
		image.data()
	);
}

template <class T>
void texture_t::update(
	const image_t<T>& image
) {
	update(0, 0, image.width(), image.height(), image.data());
}

} // End of namespace glpp::object
