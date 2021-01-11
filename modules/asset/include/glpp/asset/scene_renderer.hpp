#pragma once

#include "importer.hpp"

namespace glpp::asset {

template <class shading_model_t>
class scene_renderer_t {
public:
	
	template <class... Args>
	scene_renderer_t(const importer_t& importer, Args&&... args);
	
	void render(const render::camera_t& camera);
		
private:
	struct render_object_t {
		typename shading_model_t::view_t view;
		glm::mat4 model_matrix;
		typename shading_model_t::renderer_t renderer;
		material_t material;
	};
	
	shading_model_t m_shading_model;
	std::vector<render_object_t> m_objects;
		
	void render(render_object_t& render_object, const render::camera_t& cam) const;
};

template <class shading_model_t>
template <class... Args>
scene_renderer_t<shading_model_t>::scene_renderer_t(const importer_t& importer, Args&&... args) :
	m_shading_model(std::forward<Args>(args)...)
{
	using view_t = typename shading_model_t::view_t;
	using attribute_t = typename view_t::attribute_description_t;
	auto materials = importer.materials();
	for(const auto& mesh : importer.meshes()) {
		m_objects.emplace_back(
			render_object_t {
				view_t(
					mesh.model,
					&attribute_t::position,
					&attribute_t::normal,
					&attribute_t::tex
				),
				mesh.model_matrix,
				m_shading_model.renderer(materials[mesh.material_index]),
				materials[mesh.material_index]
			}
		);
	}
}

template <class shading_model_t>
void scene_renderer_t<shading_model_t>::render(const render::camera_t& camera) {
	for(auto& object : m_objects) {
		render(object, camera);
	}
}

template <class shading_model_t>
void scene_renderer_t<shading_model_t>::render(render_object_t& render_object, const render::camera_t& cam) const {
	using ud = typename shading_model_t::uniform_description_t;
	render_object.renderer.set_uniform(&ud::model_matrix, render_object.model_matrix);
	render_object.renderer.set_uniform(&ud::view_projection, cam.mvp());
	
	m_shading_model.set_up(render_object.renderer,render_object. material);
	render_object.renderer.render(render_object.view);
}

}
