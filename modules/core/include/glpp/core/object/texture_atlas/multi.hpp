#pragma once

#include <iterator>
#include <map>
#include <vector>
#include <algorithm>
#include <fmt/core.h>
#include "glpp/core/object/texture.hpp"

namespace glpp::core::object::texture_atlas {

class non_existant_t {
	non_existant_t() = delete;
};
class multi_policy_t {
public:
	using key_t = std::uint32_t;
	using allocation_hint_t = non_existant_t;
	using storage_t = std::map<key_t, texture_t>;
	using slot_storage_t = std::vector<texture_slot_t>;
	using swizzle_mask_t = std::array<texture_channel_t, 4>;
	using key_storage_t = std::vector<key_t>;

	multi_policy_t(
		const image_format_t format = image_format_t::rgb_8,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		const swizzle_mask_t swizzle_mask = {{texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}}
	);

	multi_policy_t(const multi_policy_t& cpy) = delete;
	multi_policy_t(multi_policy_t&& mov) noexcept = default;
	
	multi_policy_t& operator=(const multi_policy_t& cpy) = delete;
	multi_policy_t& operator=(multi_policy_t&& mov) noexcept = default;
	
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

	std::string fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const;

	key_t first_free_key() const;

	key_storage_t keys() const;

private:

	image_format_t m_format;
	clamp_mode_t m_clamp_mode;
	filter_mode_t m_filter;
	mipmap_mode_t m_mipmap_mode;
	std::array<texture_channel_t, 4> m_swizzle_mask;

	storage_t m_storage;
};

template <class PixelFormat>
multi_policy_t::key_t multi_policy_t::alloc(const key_t key, const image_t<PixelFormat>& image) {
    const auto [pos, success] = m_storage.insert({key, texture_t(image, m_format, m_clamp_mode, m_filter, m_mipmap_mode, m_swizzle_mask)});
    if(!success) {
        throw std::runtime_error("Trying to allocate subtexture with key that is already taken.");
    }
    return key;
}

template <class Image>
void multi_policy_t::update(const key_t key, const Image& image) {
    m_storage.at(key).update(image);
}

} // namespace name
