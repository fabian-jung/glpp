#pragma once

#include "glpp/glpp.hpp"
#include "glpp/object.hpp"
#include "glpp/object/attribute_properties.hpp"

#include <vector>
#include <type_traits>
#include <iterator>
#include <initializer_list>
#include <algorithm>
#include <limits>

namespace glpp::object {

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

enum class image_format_t : GLenum {
	prefered        = 0,
	red             = GL_RED,
	rg              = GL_RG,
	rgb             = GL_RGB,
	rgba            = GL_RGBA_INTEGER,
	red_i           = GL_RED_INTEGER,
	rg_i            = GL_RG_INTEGER,
	rgb_i           = GL_RGB_INTEGER,
	rgba_i          = GL_RGBA_INTEGER,
	red_8           = GL_R8,
	red_s8          = GL_R8_SNORM,
	red_16          = GL_R16,
	red_16s         = GL_R16_SNORM,
	rg_8            = GL_RG8,
	rg_8s           = GL_RG8_SNORM,
	rg_16           = GL_RG16,
	rg_16s          = GL_RG16_SNORM,
	rgb_332         = GL_R3_G3_B2,
	rgb_4           = GL_RGB4,
	rgb_5           = GL_RGB5,
	rgb_8           = GL_RGB8,
	rgb_8s          = GL_RGB8_SNORM,
	rgb_10          = GL_RGB10,
	rgb_12          = GL_RGB12,
	rgb_16          = GL_RGB16_SNORM,
	rgba_2          = GL_RGBA2,
	rgba_4          = GL_RGBA4,
	rgb_5_a1        = GL_RGB5_A1,
	rgba_8          = GL_RGBA8,
	rgba_8s         = GL_RGBA8_SNORM,
	rgb10_a2        = GL_RGB10_A2,
	rgba_10ui_a2ui  = GL_RGB10_A2UI,
	rgba_12         = GL_RGBA12,
	rgba_16         = GL_RGBA16,
	srgb8           = GL_SRGB8,
	srgba_8         = GL_SRGB8_ALPHA8,
	r_16f           = GL_R16F,
	rg_16f          = GL_RG16F,
	rgb_16f         = GL_RGB16F,
	rgba_16f        = GL_RGBA16F,
	red_32f           = GL_R32F,
	rg_32f          = GL_RG32F,
	rgb_32f         = GL_RGB32F,
	rgba_32f        = GL_RGBA32F,
	rgb_11f_11f_10f = GL_R11F_G11F_B10F,
	rgb_9_e5        = GL_RGB9_E5,
	r_8i            = GL_R8I,
	r_8ui           = GL_R8UI,
	r_16i           = GL_R16I,
	r_16ui          = GL_R16UI,
	r_32i           = GL_R32I,
	r_32ui          = GL_R32UI,
	rg_8i           = GL_RG8I,
	rg8_ui          = GL_RG8UI,
	rg_16i          = GL_RG16I,
	rg_16ui         = GL_RG16UI,
	rg_32i          = GL_RG32I,
	rg_32ui         = GL_RG32UI,
	rgb_8i          = GL_RGB8I,
	rgb_8ui         = GL_RGB8UI,
	rgb_16i         = GL_RGB16I,
	rgb_16ui        = GL_RGB16UI,
	rgb_32i         = GL_RGB32I,
	rgb_32ui        = GL_RGB32UI,
	rgba_8i         = GL_RGBA8I,
	rgba_8ui        = GL_RGBA8UI,
	rgba_16i        = GL_RGBA16I,
	rgba_16ui       = GL_RGBA16UI,
	rgba_32i        = GL_RGBA32I,
	rgba_32ui       = GL_RGBA32UI,
	r_c             = GL_COMPRESSED_RED,
	rg_c            = GL_COMPRESSED_RG,
	rgb_c           = GL_COMPRESSED_RGB,
	rgba_c          = GL_COMPRESSED_RGBA,
	rgb_s_c         = GL_COMPRESSED_SRGB,
	rgba_s_c        = GL_COMPRESSED_SRGB_ALPHA,
// 	                = GL_COMPRESSED_RED_RGTC1,
// 	                = GL_COMPRESSED_SIGNED_RED_RGTC1,
// 	                = GL_COMPRESSED_RG_RGTC2,
// 	                = GL_COMPRESSED_SIGNED_RG_RGTC2,
// 	                = GL_COMPRESSED_RGBA_BPTC_UNORM,
// 	                = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
// 	                = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
// 	                = GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
// 	                  GL_DEPTH_STENCIL
};

namespace detail {
	class stbi_image_t {
	public:
		using value_type = unsigned char;

		stbi_image_t(const char* filename, int channels);
		~stbi_image_t();
		int width() const;
		int height() const;
		const value_type* begin() const;
		const value_type* end() const;

	private:
		int               m_width;
		int               m_height;
		int               m_channels;
		value_type*       m_storage;
	};
}

template <class T>
class image_t {
public:
	using value_type = T;
	using iterator = typename std::vector<value_type>::iterator;
	using const_iterator = typename std::vector<value_type>::const_iterator;

	image_t(size_t width, size_t height);
	image_t(size_t width, size_t height, const value_type value);

	image_t(size_t width, size_t height, const value_type* begin);
	image_t(size_t width, size_t height, std::initializer_list<T> init_list);

	template <
		class Range,
		std::enable_if_t<
			std::is_same_v<typename Range::value_type, value_type>,
			int
		> = 0
	>
	image_t(size_t width, size_t height, const Range& range) :
		m_width(width),
		m_height(height),
		m_storage(range.begin(), range.end())
	{
		assert(width*height == m_storage.size());
	}

	template <
		class Iterator,
		std::enable_if_t<
			std::is_same_v<typename std::iterator_traits<Iterator>::value_type, value_type>,
			int
		> = 0
	>
	image_t(size_t width, size_t height, const Iterator& begin, const Iterator& end) :
		m_width(width),
		m_height(height),
		m_storage(begin, end)
	{
		assert(width*height == m_storage.size());
	}

	// The implementation of this constructor is in the image module. If you want to use it, you need to link
	// against glpp::image
	image_t(const char* filename);


	size_t width() const;
	size_t height() const;
	const T* data() const;
	size_t size() const;
	value_type& get(size_t x, size_t y);
	const value_type& get(size_t x, size_t y) const;
	value_type& at(size_t x, size_t y);
	const value_type& at(size_t x, size_t y) const;

	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;

	constexpr int channels() const;
	constexpr image_format_t format() const;
	constexpr GLenum type() const;

private:
	image_t(const detail::stbi_image_t& storage);

	static constexpr int channels_impl();

	const size_t m_width;
	const size_t m_height;
	std::vector<value_type> m_storage;
};

template <class Range>
image_t(size_t, size_t, const Range&) -> image_t<typename Range::value_type>;

template <class Iterator>
image_t(size_t, size_t, Iterator, Iterator) -> image_t<typename std::iterator_traits<Iterator>::value_type>;

template <class T>
image_t(size_t, size_t, std::initializer_list<T>) -> image_t<T>;

template <class T>
image_t(size_t, size_t, const T*) -> image_t<std::iterator_traits<T>>;

class texture_slot_t;

class texture_t : public object_t<> {
public:

	template <class T>
	texture_t(
		const image_t<T>& image,
		image_format_t format = image_format_t::prefered,
		const clamp_mode_t& clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t& filter = filter_mode_t::linear,
		const mipmap_mode_t& mipmap_mode = mipmap_mode_t::none
	);

	texture_slot_t bind_to_texture_slot() const;

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

private:
	static GLuint init();
	static void destroy(GLuint id);
};

class texture_slot_t {
public:
	texture_slot_t(const texture_t& texture);
	texture_slot_t(texture_slot_t&& mov);
	texture_slot_t& operator=(texture_slot_t&& mov) = delete;

	texture_slot_t(const texture_slot_t& mov) = delete;
	texture_slot_t& operator=(const texture_slot_t& mov) = delete;

	~texture_slot_t();

	GLint id() const;

private:
	int next_free_id();
	int max_texture_units() const;

	static std::vector<int> units;
	const GLuint m_id;
};

template <class T>
image_t<T>::image_t(size_t width, size_t height, std::initializer_list<T> init_list) :
	m_width(width),
	m_height(height),
	m_storage(init_list)
{
	assert(width*height == m_storage.size());
}

template <class T>
image_t<T>::image_t(size_t width, size_t height, const value_type* begin) :
	m_width(width),
	m_height(height),
	m_storage(begin, begin+width*height)
{
}

template <class T>
image_t<T>::image_t(size_t width, size_t height) :
	m_width(width),
	m_height(height),
	m_storage(width*height)
{
}

template <class T>
image_t<T>::image_t(size_t width, size_t height, const value_type value) :
	m_width(width),
	m_height(height),
	m_storage(width*height, value)
{
}

template<class T>
image_t<T>::image_t(const detail::stbi_image_t& storage) :
	m_width(storage.width()),
	m_height(storage.height()),
	m_storage(m_width*m_height)
{
	using internal_type = typename attribute_properties<T>::value_type;
	std::transform(
		storage.begin(),
		storage.end(),
		reinterpret_cast<internal_type*>(m_storage.data()),
		[](detail::stbi_image_t::value_type v) {
			using from_t = detail::stbi_image_t::value_type;
			using to_t = internal_type;

			if constexpr(
				std::is_integral_v<from_t> &&
				std::is_floating_point_v<to_t>
			) {
				return static_cast<to_t>(v)/std::numeric_limits<from_t>::max();
			} else
			if constexpr(
				std::is_floating_point_v<from_t> &&
				std::is_floating_point_v<to_t>
			) {
				return static_cast<to_t>(v);
			} else
			if constexpr(
				std::is_floating_point_v<from_t> &&
				std::is_integral_v<to_t>
			) {
				return static_cast<to_t>(v*std::numeric_limits<to_t>::max());
			} else
			if constexpr(std::is_same_v<from_t, to_t>) {
				return v;
			} else {
				// from integral to integral
				const auto normalized = static_cast<double>(v)/std::numeric_limits<from_t>::max();
				return static_cast<to_t>(
					normalized*std::numeric_limits<to_t>::max()
				);
			}
		}
	);
}

template <class T>
image_t<T>::image_t(const char* filename) :
	image_t(detail::stbi_image_t(filename, channels_impl()))
{
}

template <class T>
size_t image_t<T>::width() const {
	return m_width;
}

template <class T>
size_t image_t<T>::height() const {
	return m_height;
}

template <class T>
const T* image_t<T>::data() const {
	return m_storage.data();
}

template <class T>
size_t image_t<T>::size() const {
	return m_storage.size();
}

template <class T>
typename image_t<T>::value_type& image_t<T>::get(size_t x, size_t y) {
	return m_storage[y*m_width+x];
}

template <class T>
const typename image_t<T>::value_type& image_t<T>::get(size_t x, size_t y) const {
	return m_storage[y*m_width+x];
}

template <class T>
typename image_t<T>::value_type& image_t<T>::at(size_t x, size_t y) {
	return m_storage.at(y*m_width+x);
}

template <class T>
const typename image_t<T>::value_type& image_t<T>::at(size_t x, size_t y) const {
	return m_storage.at(y*m_width+x);
}

template <class T>
typename image_t<T>::iterator image_t<T>::begin() {
	return m_storage.begin();
}

template <class T>
typename image_t<T>::const_iterator image_t<T>::begin() const {
	return m_storage.begin();
}

template <class T>
typename image_t<T>::iterator image_t<T>::end() {
	return m_storage.end();
}

template <class T>
typename image_t<T>::const_iterator image_t<T>::end() const {
	return m_storage.end();
}

template <class T>
constexpr int image_t<T>::channels() const {
	return channels_impl();
}

template <class T>
constexpr image_format_t image_t<T>::format() const {
	if constexpr(channels_impl() == 1) {
		return image_format_t::red;
	} if constexpr(channels_impl() == 2) {
		return image_format_t::rg;
	} if constexpr(channels_impl() == 3) {
		return image_format_t::rgb;
	} if constexpr(channels_impl() == 4) {
		return image_format_t::rgba;
	}
}

template <class T>
constexpr GLenum image_t<T>::type() const {
	return attribute_properties<value_type>::type;
}

template <class T>
constexpr int image_t<T>::channels_impl() {
	return attribute_properties<value_type>::elements_per_vertex;
}

template <class T>
texture_t::texture_t(
	const image_t<T>& image,
	image_format_t format,
	const clamp_mode_t& clamp_mode,
	const filter_mode_t& filter,
	const mipmap_mode_t& mipmap_mode
) :
	object_t<>(init(), destroy)
{
	if(format == image_format_t::prefered) {
		switch(image.channels()) {
			case 1:
				format = image_format_t::red_8;
				break;
			case 2:
				format = image_format_t::rg_8;
				break;
			case 3:
				format = image_format_t::rgb_8;
				break;
			case 4:
				format = image_format_t::rgba_8;
				break;
		}
	}

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
		image.width(),
		image.height()
	);

	update(image);

	if(mipmap_mode != mipmap_mode_t::none) {
		glGenerateTextureMipmap(id());
	}

}

template <class T>
void texture_t::update(
	const image_t<T>& image,
	size_t xoffset,
	size_t yoffset,
	size_t width,
	size_t height
) {
	constexpr int level_of_detail = 0;
	glpp::call(glTextureSubImage2D,
		id(),
		level_of_detail,
		xoffset,
		yoffset,
		width,
		height,
		static_cast<GLenum>(image.format()),
		image.type(), // loading
		image.data()
	);
}

template <class T>
void texture_t::update(
	const image_t<T>& image
) {
	update(image, 0, 0, image.width(), image.height());
}

} // End of namespace glpp::object
