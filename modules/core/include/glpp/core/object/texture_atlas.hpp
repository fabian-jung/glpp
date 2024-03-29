#pragma once

#include "texture_atlas/grid.hpp"
#include "texture_atlas/multi.hpp"

namespace glpp::core::object {

template <class AllocationPolicy>
class texture_atlas_t;

template <class AllocationPolicy>
class texture_atlas_slot_t;

template <class AllocationPolicy>
class texture_atlas_entry_t {
public:
	using key_t = typename AllocationPolicy::key_t;

	texture_atlas_entry_t(texture_atlas_entry_t&& mov) = default;
	texture_atlas_entry_t& operator=(texture_atlas_entry_t&& mov) = delete;

	texture_atlas_entry_t(const texture_atlas_entry_t& cpy) = default;
	texture_atlas_entry_t& operator=(const texture_atlas_entry_t& cpy) = delete;

	template <class PixelFormat>
	void update(const image_t<PixelFormat>& image);


	template <class PixelFormat>
	texture_atlas_entry_t& operator=(const image_t<PixelFormat>& image) {
		m_alloc.update(m_key, image);
		return *this;
	}

	std::string fetch(const std::string_view name, const std::string_view uv) const;
	std::string fetch(const std::string_view name, const glm::vec2 uv) const;

	operator bool() const;
	key_t key() const;
	bool valid() const;

private:
	texture_atlas_entry_t(const AllocationPolicy& alloc, const key_t key);

	friend class texture_atlas_t<AllocationPolicy>;

	const key_t m_key;
	const AllocationPolicy& m_alloc;		
};

template <class AllocationPolicy>
class texture_atlas_t {
public:

	template <class... Args>
	explicit texture_atlas_t(Args&&... args);
	texture_atlas_t(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t(const texture_atlas_t& cpy) = delete;
	
	texture_atlas_t& operator=(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t& operator=(const texture_atlas_t& cpy) = delete;
	
	using key_t = typename AllocationPolicy::key_t;
	using allocation_hint_t = typename AllocationPolicy::allocation_hint_t;
	using key_storage_t = typename AllocationPolicy::key_storage_t;
	using entry_t = texture_atlas_entry_t<AllocationPolicy>;

	constexpr key_t to_key(allocation_hint_t key_hit) const;

	entry_t operator[](const key_t key);

	entry_t at(const key_t key);
	const entry_t at(const key_t key) const;
	
	entry_t get(const key_t key);
	const entry_t get(const key_t key) const;

	bool contains(const key_t key) const;
	bool empty() const;
	size_t size() const;
	size_t max_size() const;
	key_storage_t keys() const;

	entry_t insert();
	entry_t insert(const key_t key);
	entry_t insert(const allocation_hint_t allocation_hint);

	template <class PixelFormat>
	entry_t insert(const image_t<PixelFormat>& image);

	template <class PixelFormat>
	entry_t insert(const key_t key, const image_t<PixelFormat>& image);

	template <class PixelFormat>
	entry_t insert(const allocation_hint_t allocation_hint, const image_t<PixelFormat>& image);

	void erase(const key_t key);
	void erase(const entry_t entry);

	std::string declaration(const std::string_view name) const;

	std::string dynamic_fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const;
	std::string dynamic_fetch(const std::string_view name, const std::string_view key, const glm::vec2 uv) const;
	std::string fetch(const std::string_view name, const key_t key, const std::string_view uv) const;
	std::string fetch(const std::string_view name, const key_t key, const glm::vec2 uv) const;

	texture_atlas_slot_t<AllocationPolicy> bind_to_texture_slot() const;

private:

	AllocationPolicy m_alloc;		
};

template <class Policy>
texture_atlas_t(Policy&&) -> texture_atlas_t<Policy>;

template <class AllocationPolicy>
class texture_atlas_slot_t {
public:
	using storage_t = typename AllocationPolicy::slot_storage_t;
	using const_iterator_t = typename storage_t::const_iterator;


	texture_atlas_slot_t(texture_atlas_slot_t&& mov) = default;
	texture_atlas_slot_t& operator=(texture_atlas_slot_t&& mov) = default;

	texture_atlas_slot_t(const texture_atlas_slot_t& cpy) = delete;
	texture_atlas_slot_t& operator=(const texture_atlas_slot_t& cpy) = delete;

	const_iterator_t begin() const;
	const_iterator_t end() const;
	
private:
	friend class texture_atlas_t<AllocationPolicy>;
	texture_atlas_slot_t(storage_t storage);

	storage_t m_storage;
};

template <class AllocationPolicy>
template <class... Args>
texture_atlas_t<AllocationPolicy>::texture_atlas_t(Args&&... args) :
	m_alloc(std::forward<Args>(args)...)
{}

template <class AllocationPolicy>
template <class PixelFormat>
texture_atlas_entry_t<AllocationPolicy> texture_atlas_t<AllocationPolicy>::insert(const image_t<PixelFormat>& image) {
	return insert(m_alloc.first_free_key(), image);
}

template <class AllocationPolicy>
template <class PixelFormat>
texture_atlas_entry_t<AllocationPolicy> texture_atlas_t<AllocationPolicy>::insert(const key_t key, const image_t<PixelFormat>& image) {
	const auto allocated_key = m_alloc.alloc(key, image);
	return {m_alloc, allocated_key};
}

using grid_atlas_t = texture_atlas_t<texture_atlas::grid_policy_t>;
extern template class texture_atlas_entry_t<texture_atlas::grid_policy_t>;
extern template class texture_atlas_slot_t<texture_atlas::grid_policy_t>;
extern template class texture_atlas_t<texture_atlas::grid_policy_t>;

using multi_atlas_t = texture_atlas_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_entry_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_slot_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_t<texture_atlas::multi_policy_t>;

}