#pragma once

#include "texture_atlas/freefloat.hpp"
#include "texture_atlas/grid.hpp"
#include "texture_atlas/multi.hpp"

namespace glpp::core::object {

template <class AllocationPolicy>
class texture_atlas_slot_t;

template <class AllocationPolicy>
class texture_atlas_t {
public:

	template <class... Args>
	texture_atlas_t(const Args... args) :
		m_alloc(args...)
	{}
	
	texture_atlas_t(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t(const texture_atlas_t& cpy) = delete;
	
	texture_atlas_t& operator=(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t& operator=(const texture_atlas_t& cpy) = delete;
	
	using key_t = typename AllocationPolicy::key_t;
	using key_hint_t = typename AllocationPolicy::key_hint_t;

	template <class PixelFormat>
	key_t alloc(const image_t<PixelFormat>& image) {
		return alloc(m_alloc.first_free_key(), image);
	}

	template <class PixelFormat>
	key_t alloc(const key_hint_t key, const image_t<PixelFormat>& image) {
		return m_alloc.alloc(key, image);
	}

	key_t alloc(const key_hint_t key_hint) {
		return alloc(key_hint, image_t<glm::vec3>{1, 1});
	}

	key_t alloc() {
		return alloc(m_alloc.first_free_key());
	}

	template <class Image>
	void update(const key_t key, const Image& image) {
		m_alloc.update(key, image);
	}

	void free(const key_t key) {
		m_alloc.free(key);
	}

	auto keys() const {
		return m_alloc.keys();
	}

	std::string declaration(const std::string_view name) const {
		return m_alloc.declaration(name);
	}

	std::string fetch(const std::string_view name, const key_t key, const std::string_view uv) const {
		return fetch(name, std::to_string(key), uv);
	}

	std::string fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
		return m_alloc.fetch(name, key, uv);
	}

	std::string fetch(const std::string_view name, const key_t key, const glm::vec2 uv) const {
		return fetch(name, std::to_string(key), uv);
	}

	std::string fetch(const std::string_view name, const std::string_view key, const glm::vec2 uv) const {
		return fetch(name, key, fmt::format("vec2({}, {})", uv.x, uv.y));
	}

	texture_atlas_slot_t<AllocationPolicy> bind_to_texture_slot() const;

private:

	AllocationPolicy m_alloc;		
};

template <class AllocationPolicy>
class texture_atlas_slot_t {
public:
	explicit texture_atlas_slot_t(const typename AllocationPolicy::storage_t& storage);
	explicit texture_atlas_slot_t(const texture_atlas_t<AllocationPolicy>& atlas);

	auto begin() const;
	auto end() const;

private:
	using slot_storage_t = typename AllocationPolicy::slot_storage_t;
	slot_storage_t m_slots;
};


// template <class AllocationPolicy>
// texture_atlas_slot_t<AllocationPolicy> texture_atlas_t<AllocationPolicy>::bind_to_texture_slot() const {
// 	return texture_atlas_slot_t<AllocationPolicy>{ m_storage };
// }

// template <class AllocationPolicy>
// std::string texture_atlas_t<AllocationPolicy>::texture_id() const {
// 	return m_namespace;
// }

// template <class AllocationPolicy>
// texture_atlas_slot_t<AllocationPolicy>::texture_atlas_slot_t(const typename AllocationPolicy::storage_t& storage) :
// 	m_slots(AllocationPolicy::slots_from_storage(storage))
// {}

// template <class AllocationPolicy>
// texture_atlas_slot_t<AllocationPolicy>::texture_atlas_slot_t(const texture_atlas_t<AllocationPolicy>& atlas) :
// 	texture_atlas_slot_t(atlas.bind_to_texture_slot())
// {}

// template <class AllocationPolicy>
// auto texture_atlas_slot_t<AllocationPolicy>::begin() const {
// 	return m_slots.begin();
// }

// template <class AllocationPolicy>
// auto texture_atlas_slot_t<AllocationPolicy>::end() const {
// 	return m_slots.end();
// }

// using freefloat_atlas_t = texture_atlas_t<texture_atlas::freefloat_policy_t>;
// extern template class texture_atlas_t<texture_atlas::freefloat_policy_t>;
// using grid_atlas_t = texture_atlas_t<texture_atlas::grid_policy_t>;
// extern template class texture_atlas_t<texture_atlas::grid_policy_t>;
using multi_atlas_t = texture_atlas_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_t<texture_atlas::multi_policy_t>;

}