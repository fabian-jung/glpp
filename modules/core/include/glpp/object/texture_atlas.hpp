#pragma once

#include "texture_atlas/freefloat.hpp"
#include "texture_atlas/grid.hpp"
#include "texture_atlas/multi.hpp"

namespace glpp::object {

template <class AllocationPolicy>
class texture_atlas_slot_t;

template <class AllocationPolicy>
class texture_atlas_t {
public:

	texture_atlas_t() = default;
	
	using key_t = typename AllocationPolicy::key_t;


	template <class Image>
	key_t alloc(const Image& image) {
		return AllocationPolicy::alloc(m_storage, image);
	}
                      	
	template <class Image>
	void update(key_t key, const Image& image) {
		AllocationPolicy::update(m_storage, key, image);
	}

	std::string declaration() const {
		std::string result;
		result += "uniform sampler2D "+texture_id()+"["+std::to_string(m_storage.size())+"];\n";

		return result;
	}

	std::string fetch(const key_t key, const std::string_view uv) {
		return "texture("+texture_id(key)+", "+std::string(uv)+")";
	}

	std::string fetch(std::string_view key, const std::string_view uv);
	

	texture_atlas_slot_t<AllocationPolicy> bind_to_texture_slot() const;

	std::string texture_id() const {
		return m_namespace;
	}

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
	explicit texture_atlas_slot_t(const typename AllocationPolicy::storage_t& storage) :
		m_slots(AllocationPolicy::slots_from_storage(storage))
	{}

	explicit texture_atlas_slot_t(const texture_atlas_t<AllocationPolicy>& atlas) :
		texture_atlas_slot_t(atlas.bind_to_texture_slot())
	{}

	auto begin() const {
		return m_slots.begin();
	}

	auto end() const {
		return m_slots.end();
	}

private:
	using slot_storage_t = typename AllocationPolicy::slot_storage_t;
	slot_storage_t m_slots;
};

template <class AllocationPolicy>
texture_atlas_slot_t<AllocationPolicy> texture_atlas_t<AllocationPolicy>::bind_to_texture_slot() const {
	return texture_atlas_slot_t<AllocationPolicy>{ m_storage };
}
// using freefloat_atlas_t = texture_atlas_t<texture_atlas::freefloat_policy_t>;
// extern template class texture_atlas_t<texture_atlas::freefloat_policy_t>;
// using grid_atlas_t = texture_atlas_t<texture_atlas::grid_policy_t>;
// extern template class texture_atlas_t<texture_atlas::grid_policy_t>;
using multi_atlas_t = texture_atlas_t<texture_atlas::multi_policy_t>;
extern template class texture_atlas_t<texture_atlas::multi_policy_t>;

}