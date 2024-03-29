#include <glpp/core/object/texture_atlas/grid.hpp>


namespace glpp::core::object::texture_atlas {

grid_policy_t::grid_policy_t(
    const std::uint32_t cols,
    const std::uint32_t rows,
    const std::size_t sub_texture_width,
    const std::size_t sub_texture_height,
    const image_format_t format,
    const clamp_mode_t clamp_mode,
    const filter_mode_t filter_mode,
    const mipmap_mode_t mipmap_mode,
    const swizzle_mask_t swizzle_mask
) :
    m_rows(rows),
    m_cols(cols),
    m_width(sub_texture_width),
    m_height(sub_texture_width),
    m_keys(rows*cols, false),
    m_clamp_mode(clamp_mode),
    m_filter_mode(filter_mode),
    m_storage{
        storage_t::value_type{
            (sub_texture_width+padding())*cols,
            (sub_texture_height+padding())*rows,
            format,
            clamp_mode,
            filter_mode,
            mipmap_mode,
            swizzle_mask
        }
    }
{}

grid_policy_t::key_t grid_policy_t::position(std::uint32_t col, std::uint32_t row) const {
    return col + m_cols*row;
}

std::pair<std::uint32_t, std::uint32_t> grid_policy_t::position(const key_t key) const {
    return { key%m_cols, key/m_cols};
}

bool grid_policy_t::contains(const key_t key) const {
    return m_keys.at(key);
}

size_t grid_policy_t::size() const {
    return std::count_if(m_keys.begin(), m_keys.end(), [](bool exist){ return exist; });
}

size_t grid_policy_t::max_size() const {
    return texture_slot_t::max_texture_units();
}

void grid_policy_t::free(const key_t key) {
    if(!contains(key)) {
        throw std::runtime_error("Trying to free unallocated subtexture.");
    }
    m_keys[key] = false;
}

grid_policy_t::slot_storage_t grid_policy_t::slots() const {
    return { m_storage[0].bind_to_texture_slot() };
}

std::string grid_policy_t::texture_id(const std::string_view name, const std::string_view key) const {
    return fmt::format("{}[{}]", name, key);
}

std::string grid_policy_t::declaration(const std::string_view name) const {
    auto clamp_function = [this](clamp_mode_t clamp_mode) -> std::string {
        const glm::vec2 texel_width = glm::vec2(1.0f)/glm::vec2(m_width, m_height);
        switch(clamp_mode) {
            case clamp_mode_t::clamp_to_border:
                return fmt::format("uv+((uv-clamp(uv, vec2(-{0}, -{1}), vec2(1)+vec2({0}, {1})))*10000000000.0)", texel_width.x, texel_width.y);
            case clamp_mode_t::clamp_to_edge:
                return fmt::format(
                    "clamp(uv, 0, 1)"
                    // "vec2(clamp(uv.x, {}, {}), clamp(uv.y, {}, {}))",
                    // texel_offset.x, 1.0f-texel_offset.x, texel_offset.y, 1.0f-texel_offset.y
                );
            case clamp_mode_t::mirrored_repeat:
                return "abs(uv-round(uv/2)*2)";
            case clamp_mode_t::repeat:
                return "mod(uv, vec2(1.0))";
        }
        return "";
    }(m_clamp_mode);

    const auto sub_texture_width = glm::vec2(m_width+2, m_height+2);
    const auto border_scale = glm::vec2(m_width, m_height)/sub_texture_width;
    const auto border_offset = glm::vec2(1.0f)/sub_texture_width;

    return fmt::format(
        "uniform sampler2D {0};\n"
        "vec4 glpp_fetch_{0}(in vec2 uv, in int index) {{\n"
        "	vec2 uv_clamped = {1};\n"
        "   vec2 uv_borderless = uv_clamped*vec2({2}, {3})+vec2({4}, {5});\n"
        "	vec2 pos = vec2(mod(index, {6}), index/{6});\n"
        "	vec2 uv_boxed=(uv_borderless+pos)/vec2({6}, {7});\n"
        "	return texture({0}, uv_boxed);\n"
        "}}\n"
    , name, clamp_function, border_scale.x, border_scale.y, border_offset.x, border_offset.y, m_cols, m_rows
    );
}

std::string grid_policy_t::fetch(const std::string_view name, const std::string_view key, const std::string_view uv) const {
    return fmt::format("glpp_fetch_{}({}, {})", name, uv, key);
}

grid_policy_t::key_t grid_policy_t::first_free_key() const {
    auto first_free_it = std::find(m_keys.begin(), m_keys.end(), false);
    if(first_free_it == m_keys.end()) {
        throw std::runtime_error("bad_alloc: All texture atlas slots are already allocated.");
    }
    auto first_free_pos = std::distance(m_keys.begin(), first_free_it);
    return first_free_pos;
}

grid_policy_t::key_storage_t grid_policy_t::keys() const {
    key_storage_t result;
    result.reserve(size());
    for(auto i = 0u; i < m_keys.size(); ++i) {
        if(m_keys[i]) {
            result.emplace_back(i);
        }
    }
    return result;
}

} // namespace
