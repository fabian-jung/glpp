#pragma once
// feierabend
#include <iterator>
#include <map>
#include <vector>
#include <algorithm>
#include <fmt/core.h>
#include "glpp/core/object/texture.hpp"

namespace glpp::core::object::texture_atlas {

class grid_policy_t {
public:
	using key_t = size_t;
	using allocation_hint_t = std::pair<std::uint32_t, std::uint32_t>;
	using storage_t = std::array<texture_t, 1>;
	using slot_storage_t =  std::array<texture_slot_t, 1>;
	using swizzle_mask_t = std::array<texture_channel_t, 4>;
	using key_storage_t = std::vector<key_t>;

	grid_policy_t(
		const std::uint32_t rows,
		const std::uint32_t cols,
		const std::size_t width,
		const std::size_t height,
		const image_format_t format = image_format_t::rgb_8,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		const swizzle_mask_t swizzle_mask = {texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}
	);

	grid_policy_t(const grid_policy_t& cpy) = delete;
	grid_policy_t(grid_policy_t&& mov) noexcept = default;
	
	grid_policy_t& operator=(const grid_policy_t& cpy) = delete;
	grid_policy_t& operator=(grid_policy_t&& mov) noexcept = default;

	key_t  position(std::uint32_t col, std::uint32_t row) const;

	std::pair<std::uint32_t, std::uint32_t> position(const key_t key) const;

	bool contains(const key_t key) const;
	size_t size() const;
	size_t max_size() const;

	template <class PixelFormat>
	key_t alloc(const key_t key, const image_t<PixelFormat>& image);

	template <class Image>
	void update(const key_t key, const Image& image);
	
	void free(const key_t key);

	slot_storage_t slots() const;
	std::string texture_id(const std::string_view name, const std::string_view key) const;

	std::string declaration(const std::string_view name) const;

	std::string fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const ;
	key_t first_free_key() const;

	key_storage_t keys() const;

private:

	std::uint32_t m_rows;
	std::uint32_t m_cols;

	std::vector<bool> m_keys;
	clamp_mode_t m_clamp_mode;
	storage_t m_storage;
};

template <class PixelFormat>
grid_policy_t::key_t grid_policy_t::alloc(const key_t key, const image_t<PixelFormat>& image) {
    if(contains(key)) {
        throw std::runtime_error("Trying to allocate subtexture with key that is already taken.");
    }
    m_keys[key] = true;
    update(key, image);
    return key;
}

template <class Image>
void grid_policy_t::update(const key_t key, const Image& image) {
    // TODO rescale image
    if(
        image.width() != m_storage[0].width()/m_cols || 
        image.height() != m_storage[0].height()/m_rows
    ) {
        throw std::runtime_error("Assignemt of subtextures currently is only allowed if the size of the subtexture matches the size of the spot in the texture atlas.");
    }
    if(!contains(key)) {
        throw std::runtime_error("Trying to update subtexture with key that is not allocated.");
    }
    const auto [col, row] = position(key);
    const auto width = m_storage[0].width() / m_cols;
    const auto height = m_storage[0].height() / m_rows;
    const auto x = col * width;
    const auto y = row * height;
    m_storage[0].update(image, x, y, width, height);
}

} // namespace
