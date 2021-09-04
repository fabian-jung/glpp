#pragma once

#include <iterator>
#include <map>
#include <vector>
#include <algorithm>
#include <fmt/core.h>
#include "glpp/core/object/texture.hpp"

namespace glpp::core::object::texture_atlas {

class multi_policy_t {
public:
	using key_t = std::uint32_t;
	using key_hint_t = key_t;
	using storage_t = std::map<key_t, texture_t>;
	using slot_storage_t = std::vector<texture_slot_t>;
	using swizzle_mask_t = std::array<texture_channel_t, 4>;

	multi_policy_t(
		const image_format_t format = image_format_t::rgb_8,
		const clamp_mode_t clamp_mode = clamp_mode_t::repeat,
		const filter_mode_t filter = filter_mode_t::linear,
		const mipmap_mode_t mipmap_mode = mipmap_mode_t::none,
		const swizzle_mask_t swizzle_mask = {texture_channel_t::red, texture_channel_t::green, texture_channel_t::blue, texture_channel_t::alpha}
	) :
		m_format(format),
		m_clamp_mode(clamp_mode),
		m_filter(filter),
		m_mipmap_mode(mipmap_mode),
		m_swizzle_mask(swizzle_mask)
	{}

	size_t size() const {
		return m_storage.size();
	}

	template <class PixelFormat>
	key_t alloc(const key_t key, const image_t<PixelFormat>& image) {
		const auto [pos, success] = m_storage.insert({key, texture_t(image, m_format, m_clamp_mode, m_filter, m_mipmap_mode, m_swizzle_mask)});
		if(!success) {
			throw std::runtime_error("Trying to allocate subtexture with key that is already taken.");
		}
		return key;
	}

	template <class Image>
	void update(const key_t key, const Image& image) {
		m_storage.at(key).update(image);
	}
	
	void free(const key_t key) {
		const auto erased = m_storage.erase(key);
		if(erased == 0) {
			throw std::runtime_error("Trying to free unallocated subtexture.");
		}
	}

	slot_storage_t slots() const {
		slot_storage_t result;
		std::transform(
			m_storage.begin(),
			m_storage.end(),
			std::back_inserter(result),
			[](const auto& value) {
				const auto& [key, tex] = value;
				return tex.bind_to_texture_slot();
			}
		);
		return result;
	}

	std::string texture_id(const std::string_view name, const std::string_view key) const {
		return fmt::format("{}[{}]", name, key);
	}

	std::string declaration(const std::string_view name) const {
		if(size() > 0) {
			return fmt::format("uniform sampler2D {}[{}]", name, size());
		} else {
			return "";
		}
	}

	std::string fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
		return fmt::format("texture({}, {})", texture_id(name, key), uv);
	}

	key_t first_free_key() const {
		key_t key = 0;
		while(m_storage.count(key) > 0) {
			++key;
		}
		return key;
	}

	auto keys() const {
		std::vector<key_t> result;
		std::transform(
			m_storage.begin(),
			m_storage.end(),
			std::back_inserter(result),
			[](const auto& value){
				const auto& [key, tex] = value;
				return key;
			}
		);
		return result;
	}
private:

	image_format_t m_format;
	clamp_mode_t m_clamp_mode;
	filter_mode_t m_filter;
	mipmap_mode_t m_mipmap_mode;
	std::array<texture_channel_t, 4> m_swizzle_mask;

	storage_t m_storage;
};

} // namespace name
