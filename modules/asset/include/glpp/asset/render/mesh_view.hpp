#pragma once

#include "glpp/asset/mesh.hpp"
#include "glpp/core/render/view.hpp"

namespace glpp::asset::render {

class mesh_view_t {
public:
	using vertex_description_t = mesh_t::vertex_description_t;
	using model_t = mesh_t::model_t;
	using view_t = glpp::core::render::view_t<model_t>;

	explicit mesh_view_t(const mesh_t& mesh);

	const view_t& view() const;

	glm::mat4 model_matrix;

private:
	view_t m_view;
};

}
