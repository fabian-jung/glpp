#include "glpp/asset/shading/flat.hpp"

namespace glpp::asset::shading {

template class flat_t<core::object::texture_atlas::multi_policy_t>;
//template class flat_t<object::texture_atlas::grid_policy_t>;
//template class flat_t<object::texture_atlas::freefloat_policy_t>;

}