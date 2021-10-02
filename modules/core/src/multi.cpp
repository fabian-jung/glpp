#include <glpp/core/object/texture_atlas/multi.hpp>

namespace glpp::core::object::texture_atlas {

multi_policy_t::multi_policy_t(
    const image_format_t format,
    const clamp_mode_t clamp_mode,
    const filter_mode_t filter,
    const mipmap_mode_t mipmap_mode,
    const swizzle_mask_t swizzle_mask
) :
    m_format(format),
    m_clamp_mode(clamp_mode),
    m_filter(filter),
    m_mipmap_mode(mipmap_mode),
    m_swizzle_mask(swizzle_mask)
{}

bool multi_policy_t::contains(const key_t key) const {
    return m_storage.contains(key);
}

size_t multi_policy_t::size() const {
    return m_storage.size();
}

size_t multi_policy_t::max_size() const {
    return texture_slot_t::max_texture_units();
}

void multi_policy_t::free(const key_t key) {
    const auto erased = m_storage.erase(key);
    if(erased == 0) {
        throw std::runtime_error("Trying to free unallocated subtexture.");
    }
}

multi_policy_t::slot_storage_t multi_policy_t::slots() const {
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

std::string multi_policy_t::texture_id(const std::string_view name, const std::string_view key) const {
    return fmt::format("{}[{}]", name, key);
}

std::string multi_policy_t::declaration(const std::string_view name) const {
    if(size() > 0) {
        return fmt::format("uniform sampler2D {}[{}]", name, size());
    } else {
        return "";
    }
}

std::string multi_policy_t::fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
    return fmt::format("texture({}, {})", texture_id(name, key), uv);
}

multi_policy_t::key_t multi_policy_t::first_free_key() const {
    key_t key = 0;
    while(m_storage.count(key) > 0) {
        ++key;
    }
    return key;
}

multi_policy_t::key_storage_t multi_policy_t::keys() const {
    key_storage_t result;
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

}