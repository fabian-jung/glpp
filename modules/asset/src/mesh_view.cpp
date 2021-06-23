#include <glpp/asset/render/mesh_view.hpp>

namespace glpp::asset::render {

mesh_view_t::mesh_view_t(const mesh_t& mesh) :
    model_matrix(mesh.model_matrix),
    m_view(
        mesh.model,
        &vertex_description_t::position,
        &vertex_description_t::normal,
        &vertex_description_t::tex
    )
{}

const mesh_view_t::view_t& mesh_view_t::view() const {
    return m_view;
}

}