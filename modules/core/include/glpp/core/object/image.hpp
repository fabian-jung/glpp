#pragma once

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <ostream>
#include <typeinfo>
#include <glm/gtx/string_cast.hpp>
#include <glpp/core/glpp.hpp>
#include <glpp/core/object/attribute_properties.hpp>
#include <functional>

namespace glpp::core::object {

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
	red_32f         = GL_R32F,
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
	d_32f           = GL_DEPTH_COMPONENT32F,
	d_24i           = GL_DEPTH_COMPONENT24,
	d_16i           = GL_DEPTH_COMPONENT16,
	d_32f_s_8i      = GL_DEPTH32F_STENCIL8,
	d_24i_s_8i      = GL_DEPTH24_STENCIL8,
	s_8i            = GL_STENCIL_INDEX8,
};

namespace detail {
	class stbi_image_t {
	public:
		using value_type = unsigned char;

		stbi_image_t(const char* filename, int channels);
		stbi_image_t(size_t width, size_t height, int channels);
		~stbi_image_t();

		int width() const;
		int height() const;

		void write(const char* filename) const;

		const value_type* begin() const;
		const value_type* end() const;

		value_type* begin();
		value_type* end();

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

	constexpr image_t() = default;
	explicit constexpr image_t(const image_t& cpy) = default;
	constexpr image_t(image_t&& mov) noexcept = default;

	template <class U>
	explicit constexpr image_t(const image_t<U>& conv);

	constexpr image_t& operator=(const image_t& cpy) = default;
	constexpr image_t& operator=(image_t&& mov) noexcept = default;

	constexpr image_t(size_t width, size_t height);
	constexpr image_t(size_t width, size_t height, const value_type value);

	constexpr image_t(size_t width, size_t height, const value_type* begin);
	constexpr image_t(size_t width, size_t height, std::initializer_list<T> init_list);

	template <
		class Range,
		std::enable_if_t<
			std::is_same_v<typename Range::value_type, value_type>,
			int
		> = 0
	>
	constexpr image_t(size_t width, size_t height, const Range& range) :
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
	constexpr image_t(size_t width, size_t height, const Iterator& begin, const Iterator& end) :
		m_width(width),
		m_height(height),
		m_storage(begin, end)
	{
		assert(width*height == m_storage.size());
	}

	// The implementation of this constructor is in the image module. If you want to use it, you need to link
	// against glpp::image
	image_t(const char* filename);


	void load(const char* filename);
	void write(const char* filename) const;

	constexpr size_t width() const noexcept;
	constexpr size_t height() const noexcept;
	constexpr T* data() noexcept;
	constexpr const T* data() const noexcept;
	constexpr size_t size() const noexcept;
	constexpr value_type& get(size_t x, size_t y);
	constexpr const value_type& get(size_t x, size_t y) const;
	constexpr value_type& at(size_t x, size_t y);
	constexpr const value_type& at(size_t x, size_t y) const;
 
	constexpr iterator begin() noexcept;
	constexpr const_iterator begin() const noexcept;
	constexpr iterator end() noexcept;
	constexpr const_iterator end() const noexcept;

	constexpr int channels() const noexcept;
	constexpr image_format_t format() const noexcept;
	constexpr GLenum type() const noexcept;

private:

	static constexpr int channels_impl() noexcept;
	
	template <class From, class To>
	static constexpr To convert_pixel_format(From v);

	size_t m_width { 0 };
	size_t m_height { 0 };
	std::vector<value_type> m_storage {};
};

template <class T>
image_t(size_t, size_t, T) -> image_t<T>;

template <class T>
image_t(size_t, size_t, T*) -> image_t<T>;

template <class Iterator>
image_t(size_t, size_t, Iterator, Iterator) -> image_t<typename std::iterator_traits<Iterator>::value_type>;

template <class T>
image_t(size_t, size_t, std::initializer_list<T>) -> image_t<T>;

template <class T>
image_t(size_t, size_t, const T*) -> image_t<std::iterator_traits<T>>;

template <class T>
struct epsilon_matcher_t {
	constexpr bool operator()(const T& lhs, const T& rhs) const {
		if constexpr(std::is_arithmetic_v<T>) {
			return glm::abs(lhs-rhs) <= epsilon;
		} else {
			return glm::all(glm::lessThanEqual(glm::abs(lhs-rhs), T(epsilon)));
		}
	}

	using value_type = typename attribute_properties<T>::value_type;
	value_type epsilon = static_cast<value_type>(0.01);
};
template <class T, class Comperator = std::equal_to<T>>
struct image_comperator_t {
	constexpr operator bool() const {
		return 
			lhs.width() == rhs.width() &&
			lhs.width() == rhs.width() &&
			std::equal(lhs.begin(), lhs.end(), rhs.begin(), comperator);
	}

	constexpr auto epsilon(typename attribute_properties<T>::value_type epsilon) {
		return image_comperator_t<T, epsilon_matcher_t<T>>{lhs, rhs, { epsilon }};
	}
	const image_t<T>& lhs;
	const image_t<T>& rhs;
	Comperator comperator {};
};

template <class T, class Comp>
std::ostream& operator<<(std::ostream& lhs, const image_comperator_t<T, Comp>& rhs) {
	lhs << rhs.lhs << " == " << rhs.rhs;
	return lhs;
}

template <class T>
image_comperator_t(const image_t<T>&, const image_t<T>&) -> image_comperator_t<T>;

template <class T>
constexpr auto operator==(const image_t<T>& lhs, const image_t<T>& rhs) {
	return image_comperator_t{lhs, rhs};
} 

template <class T>
std::ostream& operator<<(std::ostream& lhs, const image_t<T>& rhs) {
    lhs << "image_t<" << typeid(T).name() <<"> { " << rhs.width() << ", " << rhs.height() << ", {";
    constexpr size_t max_display_elem = 9;
    const auto cpy_n = std::min(rhs.size(), max_display_elem);
    std::for_each_n(rhs.begin(), cpy_n-1, [&](const auto& elem){
		if constexpr(std::is_arithmetic_v<T>) {
			lhs << elem << ", ";
		} else {
	        lhs << glm::to_string(elem) << ", ";
		}
    });
    if(rhs.size() >= cpy_n) {
		if constexpr(std::is_arithmetic_v<T>) {
			lhs << rhs.begin()[cpy_n-1];
		} else {
	        lhs << glm::to_string(rhs.begin()[cpy_n-1]);
		}
    }
    if(rhs.size() > max_display_elem) {
        lhs << ", ... ";
    }
    lhs << "} }";
    return lhs;
}

template <class T>
template <class U>
constexpr image_t<T>::image_t(const image_t<U>& conv) :
	m_width(conv.width()),
	m_height(conv.height()),
	m_storage(conv.size())
{
	static_assert(
		attribute_properties<T>::elements_per_vertex == attribute_properties<U>::elements_per_vertex,
		"Conversion between image formats is only possible, if the number of channels match exactly."
	);
	std::transform(
		conv.begin(),
		conv.end(),
		begin(),
		[](auto u) {
			if constexpr (std::is_arithmetic_v<decltype(u)>) {
				return convert_pixel_format<U, T>(u);
			} else {
				T result;
				for(auto i = 0; i < attribute_properties<T>::elements_per_vertex; ++i) {
					result[i] = convert_pixel_format<attribute_properties<U>::value_type, attribute_properties<T>::value_type>(u[i]);
				}
				return result;
			}
		}
	);
}

template <class T>
constexpr image_t<T>::image_t(size_t width, size_t height, std::initializer_list<T> init_list) :
	m_width(width),
	m_height(height),
	m_storage(init_list)
{
	assert(width*height == m_storage.size());
}

template <class T>
constexpr image_t<T>::image_t(size_t width, size_t height, const value_type* begin) :
	m_width(width),
	m_height(height),
	m_storage(begin, begin+width*height)
{
}

template <class T>
constexpr image_t<T>::image_t(size_t width, size_t height) :
	m_width(width),
	m_height(height),
	m_storage(width*height)
{
}

template <class T>
constexpr image_t<T>::image_t(size_t width, size_t height, const value_type value) :
	m_width(width),
	m_height(height),
	m_storage(width*height, value)
{
}

template <class T>
image_t<T>::image_t(const char* filename)
{
	load(filename);
}

template <class T>
void image_t<T>::load(const char* filename)
{
	const auto storage = detail::stbi_image_t(filename, channels_impl());
	m_width = storage.width();
	m_height = storage.height();
	m_storage.resize(m_width*m_height);
	using internal_type = typename attribute_properties<T>::value_type;
	std::transform(
		storage.begin(),
		storage.end(),
		reinterpret_cast<internal_type*>(m_storage.data()),
		convert_pixel_format<detail::stbi_image_t::value_type, internal_type>
	);
}

template <class T>
void image_t<T>::write(const char* filename) const {
	auto storage = detail::stbi_image_t(m_width, m_height, channels_impl());
	using internal_type = typename attribute_properties<T>::value_type;
	std::transform(
		reinterpret_cast<const internal_type*>(data()),
		reinterpret_cast<const internal_type*>(data())+size()*channels_impl(),
		storage.begin(),
		convert_pixel_format<internal_type, detail::stbi_image_t::value_type>
	);
	storage.write(filename);
}

template <class T>
constexpr size_t image_t<T>::width() const noexcept {
	return m_width;
}

template <class T>
constexpr size_t image_t<T>::height() const noexcept{
	return m_height;
}

template <class T>
constexpr T* image_t<T>::data() noexcept {
	return m_storage.data();
}

template <class T>
constexpr const T* image_t<T>::data() const noexcept {
	return m_storage.data();
}

template <class T>
constexpr size_t image_t<T>::size() const noexcept {
	return m_storage.size();
}

template <class T>
constexpr typename image_t<T>::value_type& image_t<T>::get(size_t x, size_t y) {
	return m_storage[y*m_width+x];
}

template <class T>
constexpr const typename image_t<T>::value_type& image_t<T>::get(size_t x, size_t y) const {
	return m_storage[y*m_width+x];
}

template <class T>
constexpr typename image_t<T>::value_type& image_t<T>::at(size_t x, size_t y) {
	if(x >= width() || y >= height()) {
		const auto msg = std::string("")+
			"Try to access image at ["+std::to_string(x)+", "+std::to_string(y)+"]"
			" with the dimensions of ["+std::to_string(width())+", "+std::to_string(height())+"].";
		throw std::out_of_range(msg);
	}
	return get(x, y);
}

template <class T>
constexpr const typename image_t<T>::value_type& image_t<T>::at(size_t x, size_t y) const {
	if(x >= width() || y >= height()) {
		const auto msg = std::string("")+
			"Try to access image at ["+std::to_string(x)+", "+std::to_string(y)+"]"
			" with the dimensions of ["+std::to_string(width())+", "+std::to_string(height())+"].";
		throw std::out_of_range(msg);	
	}
	return get(x, y);
}

template <class T>
constexpr typename image_t<T>::iterator image_t<T>::begin() noexcept {
	return m_storage.begin();
}

template <class T>
constexpr typename image_t<T>::const_iterator image_t<T>::begin() const noexcept {
	return m_storage.begin();
}

template <class T>
constexpr typename image_t<T>::iterator image_t<T>::end() noexcept {
	return m_storage.end();
}

template <class T>
constexpr typename image_t<T>::const_iterator image_t<T>::end() const noexcept {
	return m_storage.end();
}

template <class T>
constexpr int image_t<T>::channels() const noexcept {
	return channels_impl();
}

template <class T>
constexpr image_format_t image_t<T>::format() const noexcept {
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
constexpr GLenum image_t<T>::type() const noexcept {
	return attribute_properties<value_type>::type;
}

template <class T>
constexpr int image_t<T>::channels_impl() noexcept {
	return attribute_properties<value_type>::elements_per_vertex;
}

template <class T>
template <class From, class To>
constexpr To image_t<T>::convert_pixel_format(From v) {
	if constexpr(
		std::is_integral_v<From> &&
		std::is_floating_point_v<To>
	) {
		constexpr auto max = static_cast<To>(std::numeric_limits<From>::max());
		constexpr auto min = static_cast<To>(std::numeric_limits<From>::min());
		constexpr auto increments = max-min;
		return (static_cast<To>(v)-min)/increments;
	} else 
	if constexpr(
		std::is_floating_point_v<From> &&
		std::is_floating_point_v<To>
	) {
		return static_cast<To>(v);
	} else 
	if constexpr(
		std::is_floating_point_v<From> &&
		std::is_integral_v<To>
	) {
		constexpr auto max = static_cast<From>(std::numeric_limits<To>::max());
		constexpr auto min = static_cast<From>(std::numeric_limits<To>::min());
		constexpr auto increments = max-min;
		return static_cast<To>(std::ceil(v*increments+min));
	} else 
	if constexpr(std::is_same_v<From, To>) {
		return v;
	} else {
		// from integral to integral
		using normalized_t = double;
		const auto normalized = convert_pixel_format<From, normalized_t>(v);
		return convert_pixel_format<normalized_t, To>(normalized);
	}
}
	
}