#pragma once
// feierabend
#include <glm/fwd.hpp>
#include <iterator>
#include <map>
#include <vector>
#include <algorithm>
#include <fmt/core.h>
#include "glpp/core/object/texture.hpp"

namespace glpp::core::object::texture_atlas {

class grid_policy_t {
public:
	using key_t = std::uint32_t;
	using allocation_hint_t = std::pair<std::uint32_t, std::uint32_t>;
	using storage_t = std::array<texture_t, 1>;
	using slot_storage_t =  std::array<texture_slot_t, 1>;
	using swizzle_mask_t = std::array<texture_channel_t, 4>;
	using key_storage_t = std::vector<key_t>;

	grid_policy_t(
		const std::uint32_t cols,
		const std::uint32_t rows,
		const std::size_t sub_texture_width,
		const std::size_t sub_texture_height,
		const image_format_t format = image_format_t::rgb_8,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		const swizzle_mask_t swizzle_mask = {{texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}}
	);

	grid_policy_t(const grid_policy_t& cpy) = delete;
	grid_policy_t(grid_policy_t&& mov) noexcept = default;
	
	grid_policy_t& operator=(const grid_policy_t& cpy) = delete;
	grid_policy_t& operator=(grid_policy_t&& mov) noexcept = default;

	key_t position(std::uint32_t col, std::uint32_t row) const;

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
	
	size_t padding() const {
		// return m_filter_mode == filter_mode_t::linear ? 2 : 0;
		return 2;
	}

	template <class Image>
	Image prepare(const Image& image) const {
		auto resized = image.resize(m_width, m_height);
		if(padding() == 2) {
			Image padded { m_width+2, m_height+2};
			padded.update(1,1,resized);
			const auto bottom = m_height+1;
			const auto right = m_width+1;
			switch(m_clamp_mode) {
				case clamp_mode_t::clamp_to_border:{
					glm::vec4 color(0.0f);
					// TODO
					// glGetTexParameterfv(m_storage[0].id(), GL_TEXTURE_BORDER_COLOR, &color[0]);
					for(auto x = 0u; x < m_width+2; ++x) {
						padded.get(x,0) = color;
						padded.get(x,bottom) = color;
					}
					for(auto y = 1u; y < m_height+1; ++y) {
						padded.get(0,y) = color;
						padded.get(right,y) = color;
					}
					break;
				}
				case clamp_mode_t::clamp_to_edge:
				case clamp_mode_t::mirrored_repeat :
					for(auto x = 1u; x < m_width+1; ++x) {
						padded.get(x,0) = padded.get(x,1);
						padded.get(x,bottom) = padded.get(x,bottom-1);
					}
					for(auto y = 0u; y < m_height+2; ++y) {
						padded.get(0,y) = padded.get(1,y);
						padded.get(right,y) = padded.get(right-1,y);
					}
				break;
				case clamp_mode_t::repeat:
					for(auto x = 1u; x < m_width+1; ++x) {
						padded.get(x,0) = padded.get(x,bottom-1);
						padded.get(x,bottom) = padded.get(x,1);
					}
					for(auto y = 0u; y < m_height+2; ++y) {
						padded.get(0,y) = padded.get(right-1,y);
						padded.get(right,y) = padded.get(1,y);
					}
				break;
				default:
					throw std::runtime_error("not implemented");				
			}
			return padded;
		} else {
			return resized;
		}
	}

	std::uint32_t m_rows;
	std::uint32_t m_cols;
	size_t m_width;
	size_t m_height;

	std::vector<bool> m_keys;
	clamp_mode_t m_clamp_mode;
	filter_mode_t m_filter_mode;
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

    if(!contains(key)) {
        throw std::runtime_error("Trying to update subtexture with key that is not allocated.");
    }

	const auto prepared = prepare(image);
	
    const auto [col, row] = position(key);
    const auto width = prepared.width();
    const auto height = prepared.height();
    const auto x = col * width;
    const auto y = row * height;
    m_storage[0].update(prepared, x, y, width, height);
}

} // namespace
