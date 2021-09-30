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
	) :
		m_rows(rows),
		m_cols(cols),
		m_keys(rows*cols, false),
		m_clamp_mode(clamp_mode),
		m_storage_range{
			storage_t::value_type{
				width,
				height,
				format,
				clamp_mode,
				filter,
				mipmap_mode,
				swizzle_mask
			}
		}
	{}

	grid_policy_t(const grid_policy_t& cpy) = delete;
	grid_policy_t(grid_policy_t&& mov) noexcept = default;
	
	grid_policy_t& operator=(const grid_policy_t& cpy) = delete;
	grid_policy_t& operator=(grid_policy_t&& mov) noexcept = default;

	key_t  position(std::uint32_t col, std::uint32_t row) const {
		return col + m_cols*row;
	}

	std::pair<std::uint32_t, std::uint32_t> position(const key_t key) const {
		return { key%m_cols, key/m_cols};
	}

	bool contains(const key_t key) const {
		return m_keys.at(key);
	}

	auto size() const {
		return std::count_if(m_keys.begin(), m_keys.end(), [](bool exist){ return exist; });
	}

	auto max_size() const {
		return texture_slot_t::max_texture_units();
	}

	template <class PixelFormat>
	key_t alloc(const key_t key, const image_t<PixelFormat>& image) {
		if(contains(key)) {
			throw std::runtime_error("Trying to allocate subtexture with key that is already taken.");
		}
		m_keys[key] = true;
		update(key, image);
		return key;
	}

	template <class Image>
	void update(const key_t key, const Image& image) {
		// TODO rescale image
		if(
			image.width() != m_storage.width()/m_cols || 
			image.height() != m_storage.height()/m_rows
		) {
			throw std::runtime_error("Assignemt of subtextures currently is only allowed if the size of the subtexture matches the size of the spot in the texture atlas.");
		}
		if(!contains(key)) {
			throw std::runtime_error("Trying to update subtexture with key that is not allocated.");
		}
		const auto [col, row] = position(key);
		const auto width = m_storage.width() / m_cols;
		const auto height = m_storage.height() / m_rows;
		const auto x = col * width;
		const auto y = row * height;
		m_storage.update(image, x, y, width, height);
	}
	
	void free(const key_t key) {
		if(!contains(key)) {
			throw std::runtime_error("Trying to free unallocated subtexture.");
		}
		m_keys[key] = false;
	}

	slot_storage_t slots() const {
		return { m_storage.bind_to_texture_slot() };
	}

	std::string texture_id(const std::string_view name, const std::string_view key) const {
		return fmt::format("{}[{}]", name, key);
	}

	std::string declaration(const std::string_view name) const {
		auto clamp_function = [this](clamp_mode_t clamp_mode) -> std::string {
			const glm::vec2 texel_offset(1.0f/static_cast<float>(m_storage.width()), 1.0f/static_cast<float>(m_storage.height()));
			switch(clamp_mode) {
				case clamp_mode_t::clamp_to_border:
					return "uv+((uv-clamp(uv, 0, 1))*10000000000.0)";
				case clamp_mode_t::clamp_to_edge:
					return fmt::format(
						"vec2(clamp(uv.x, {}, {}), clamp(uv.y, {}, {}))",
						texel_offset.x, 1.0f-texel_offset.x, texel_offset.y, 1.0f-texel_offset.y
					);
				case clamp_mode_t::mirrored_repeat:
					return "abs(uv-round(uv/2)*2)";
				case clamp_mode_t::repeat:
					return "mod(uv, vec2(1.0))";
			}
			return "";
		}(m_clamp_mode);
		return fmt::format(
			"uniform sampler2D {};\n"
			"vec4 glpp_fetch_{}(in vec2 uv, in int index) {{\n"
			"	vec2 uv_clamped = {};\n"
			"	vec2 pos = vec2(mod(index, {}), index/{});\n"
			"	vec2 uv_boxed=(uv_clamped+pos)/vec2({}, {});\n"
			"	return texture({}, uv_boxed);\n"
			"}}\n"
		, name, name, clamp_function, m_cols, m_cols, m_cols, m_rows, name
		);
	}

	std::string fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
		return fmt::format("glpp_fetch_{}({}, {})", name, uv, key);
	}

	key_t first_free_key() const {
		auto first_free_it = std::find(m_keys.begin(), m_keys.end(), false);
		if(first_free_it == m_keys.end()) {
			throw std::runtime_error("bad_alloc: All texture atlas slots are already allocated.");
		}
		auto first_free_pos = std::distance(m_keys.begin(), first_free_it);
		return first_free_pos;
	}

	key_storage_t keys() const {
		key_storage_t result;
		result.reserve(size());
		for(auto i = 0u; i < m_keys.size(); ++i) {
			if(m_keys[i]) {
				result.emplace_back(i);
			}
		}
		return result;
	}

private:

	std::uint32_t m_rows;
	std::uint32_t m_cols;

	std::vector<bool> m_keys;
	clamp_mode_t m_clamp_mode;
	storage_t m_storage_range;
	storage_t::value_type& m_storage = m_storage_range.front();
};

} // namespace
