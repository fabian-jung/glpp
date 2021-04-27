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

	texture_atlas_t() = default;
	
	texture_atlas_t(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t(const texture_atlas_t& cpy) = delete;
	
	texture_atlas_t& operator=(texture_atlas_t&& mov) noexcept = default;
	texture_atlas_t& operator=(const texture_atlas_t& cpy) = delete;
	
	using key_t = typename AllocationPolicy::key_t;


	template <class Image>
	key_t alloc(const Image& image);
                      	
	template <class Image>
	void update(key_t key, const Image& image);

	std::string declaration() const;
	std::string fetch(const key_t key, const std::string_view uv) const;
	std::string fetch(std::string_view key, const std::string_view uv) const;
	

	texture_atlas_slot_t<AllocationPolicy> bind_to_texture_slot() const;

	std::string texture_id() const;

private:

	std::string texture_id(const key_t key) const {
		return m_namespace+"["+std::to_string(key)+"]";
	}
	std::string m_namespace = "texa";
	
	using texture_storage_t = typename AllocationPolicy::storage_t;
	texture_storage_t m_storage;
	
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

template <class AllocationPolicy>
template <class Image>
typename texture_atlas_t<AllocationPolicy>::key_t 
texture_atlas_t<AllocationPolicy>::alloc(const Image& image) {
	return AllocationPolicy::alloc(m_storage, image);
}

template <class AllocationPolicy>					
template <class Image>
void texture_atlas_t<AllocationPolicy>::update(key_t key, const Image& image) {
	AllocationPolicy::update(m_storage, key, image);
}

template <class AllocationPolicy>
std::string texture_atlas_t<AllocationPolicy>::declaration() const {
	std::string result;
	if(AllocationPolicy::size(m_storage) > 0) {
		result += "uniform sampler2D "+texture_id()+"["+std::to_string(m_storage.size())+"];\n";
	}
	return result;
}

template <class AllocationPolicy>
std::string texture_atlas_t<AllocationPolicy>::fetch(const key_t key, const std::string_view uv) const {
	return "texture("+texture_id(key)+", "+std::string(uv)+")";
}

template <class AllocationPolicy>
texture_atlas_slot_t<AllocationPolicy> texture_atlas_t<AllocationPolicy>::bind_to_texture_slot() const {
	return texture_atlas_slot_t<AllocationPolicy>{ m_storage };
}

template <class AllocationPolicy>
std::string texture_atlas_t<AllocationPolicy>::texture_id() const {
	return m_namespace;
}

template <class AllocationPolicy>
texture_atlas_slot_t<AllocationPolicy>::texture_atlas_slot_t(const typename AllocationPolicy::storage_t& storage) :
	m_slots(AllocationPolicy::slots_from_storage(storage))
{}

template <class AllocationPolicy>
texture_atlas_slot_t<AllocationPolicy>::texture_atlas_slot_t(const texture_atlas_t<AllocationPolicy>& atlas) :
	texture_atlas_slot_t(atlas.bind_to_texture_slot())
{}

template <class AllocationPolicy>
auto texture_atlas_slot_t<AllocationPolicy>::begin() const {
	return m_slots.begin();
}

template <class AllocationPolicy>
auto texture_atlas_slot_t<AllocationPolicy>::end() const {
	return m_slots.end();
}

// using freefloat_atlas_t = texture_atlas_t<texture_atlas::freefloat_policy_t>;
// extern template class texture_atlas_t<texture_atlas::freefloat_policy_t>;
// using grid_atlas_t = texture_atlas_t<texture_atlas::grid_policy_t>;
// extern template class texture_atlas_t<texture_atlas::grid_policy_t>;
using multi_atlas_t = texture_atlas_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_t<texture_atlas::multi_policy_t>;

}