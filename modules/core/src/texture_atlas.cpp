#include "glpp/core/object/texture_atlas.hpp"

namespace glpp::core::object
{

template <class AllocPolicy>
template <class PixelFormat>
void texture_atlas_entry_t<AllocPolicy>::update(const image_t<PixelFormat>& image) {
    m_alloc.update(m_key, image);
}

/**
@brief check if texture_entry_t object is still contained in the texture_atlas_t object

This operator has the same implementation as texture_atlas_entry_t::valid().
*/
template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy>::operator bool() const {
    return valid();
}	

template <class AllocPolicy>
std::string texture_atlas_entry_t<AllocPolicy>::fetch(const std::string_view name, const std::string_view uv) const {
    return m_alloc.fetch(name, std::to_string(m_key), uv);
}

template <class AllocPolicy>
std::string texture_atlas_entry_t<AllocPolicy>::fetch(const std::string_view name, const glm::vec2 uv) const {
    return fetch(name, fmt::format("vec2({}, {})", uv.x, uv.y));
}

template <class AllocPolicy>
typename texture_atlas_entry_t<AllocPolicy>::key_t texture_atlas_entry_t<AllocPolicy>::key() const {
    return m_key;
}

template <class AllocPolicy>
bool texture_atlas_entry_t<AllocPolicy>::valid() const {
    return m_alloc.contains(key());
}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy>::texture_atlas_entry_t(const AllocPolicy& alloc, const key_t key) :
    m_key(key),
    m_alloc(alloc)
{}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::operator[](const key_t key) {
    if(contains(key)) {
        return {m_alloc, key};
    }
    return insert(key);
}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::at(const key_t key) {
    if(!contains(key)) {
        throw std::runtime_error("The requested key is not contained in texture_atlas.");
    }
    return {m_alloc, key};
}

template <class AllocPolicy>
const texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::at(const key_t key) const {
    if(!contains(key)) {
        throw std::runtime_error("The requested key is not contained in texture_atlas.");
    }
    return {m_alloc, key};
}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::get(const key_t key) {
    return {m_alloc, key};
}

template <class AllocPolicy>
const texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::get(const key_t key) const {
    return {m_alloc, key};
}

template <class AllocPolicy>
bool texture_atlas_t<AllocPolicy>::contains(const key_t key) const {
    return m_alloc.contains(key);
}

template <class AllocPolicy>
bool texture_atlas_t<AllocPolicy>::empty() const {
    return m_alloc.size() == 0;
}

template <class AllocPolicy>
size_t texture_atlas_t<AllocPolicy>::size() const {
    return m_alloc.size();
}

template <class AllocPolicy>
size_t texture_atlas_t<AllocPolicy>::max_size() const {
    return m_alloc.max_size();
}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::insert() {
    return insert(m_alloc.first_free_key());
}

template <class AllocPolicy>
texture_atlas_entry_t<AllocPolicy> texture_atlas_t<AllocPolicy>::insert(const key_hint_t key_hint) {
    return insert(key_hint, image_t<glm::vec3>{1, 1});
}

template <class AllocPolicy>
void texture_atlas_t<AllocPolicy>::erase(const key_t key) {
    m_alloc.free(key);
}

template <class AllocPolicy>
void texture_atlas_t<AllocPolicy>::erase(const texture_atlas_entry_t<AllocPolicy> entry) {
    erase(entry.key());
}

template <class AllocPolicy>
typename texture_atlas_t<AllocPolicy>::key_storage_t texture_atlas_t<AllocPolicy>::keys() const {
    return m_alloc.keys();
}

template <class AllocPolicy>
std::string texture_atlas_t<AllocPolicy>::declaration(const std::string_view name) const {
    return m_alloc.declaration(name);
}

template <class AllocPolicy>
std::string texture_atlas_t<AllocPolicy>::dynamic_fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
    return m_alloc.fetch(name, key, uv);
}

template <class AllocPolicy>
std::string texture_atlas_t<AllocPolicy>::dynamic_fetch(const std::string_view name, const std::string_view key, const glm::vec2 uv) const {
    return dynamic_fetch(name, key, fmt::format("vec2({}, {})", uv.x, uv.y));
}

template <class AllocPolicy>
std::string texture_atlas_t<AllocPolicy>::fetch(const std::string_view name, const key_t key, const std::string_view uv) const {
    return m_alloc.fetch(name, std::to_string(key), uv);
}

template <class AllocPolicy>
std::string texture_atlas_t<AllocPolicy>::fetch(const std::string_view name, const key_t key, const glm::vec2 uv) const {
    return fetch(name, key, fmt::format("vec2({}, {})", uv.x, uv.y));
}

template <class AllocPolicy>
texture_atlas_slot_t<AllocPolicy> texture_atlas_t<AllocPolicy>::bind_to_texture_slot() const {
    return { m_alloc.slots() };
}

template <class AllocationPolicy>
texture_atlas_slot_t<AllocationPolicy>::texture_atlas_slot_t(storage_t storage) :
    m_storage(std::move(storage))
{}

template <class AllocationPolicy>
typename texture_atlas_slot_t<AllocationPolicy>::const_iterator_t texture_atlas_slot_t<AllocationPolicy>::begin() const {
    return m_storage.begin();
}

template <class AllocationPolicy>
typename texture_atlas_slot_t<AllocationPolicy>::const_iterator_t texture_atlas_slot_t<AllocationPolicy>::end() const {
    return m_storage.end();
}

// template class texture_atlas_entry_t<texture_atlas::freefloat_policy_t>;
// template class texture_atlas_slot_t<texture_atlas::freefloat_policy_t>;
// template class texture_atlas_t<texture_atlas::freefloat_policy_t>;

// template class texture_atlas_entry_t<texture_atlas::grid_policy_t>;
// template class texture_atlas_slot_t<texture_atlas::grid_policy_t>;
// template class texture_atlas_t<texture_atlas::grid_policy_t>;

template class texture_atlas_entry_t<texture_atlas::multi_policy_t>;
template class texture_atlas_slot_t<texture_atlas::multi_policy_t>;
template class texture_atlas_t<texture_atlas::multi_policy_t>;

}