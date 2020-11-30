#pragma once

#include "mesh_view.hpp"
#include <glpp/core/render/camera.hpp>

namespace glpp::asset::render {

template <class ShadingModel>
class mesh_renderer_t {
public:
	using renderer_t = typename ShadingModel::renderer_t;
	using uniform_description_t = typename ShadingModel::uniform_description_t;

	explicit mesh_renderer_t(ShadingModel model, const material_t& material);

	template <class T>
	void set_uniform(T uniform_description_t::* uniform, T value);

	void update_model_matrix(const glm::mat4& model_matrix);
	void update_projection(const glm::mat4& model_matrix);
	void update_projection(const core::render::camera_t& camera);

	template <class AllocPolicy>
	void update_texture_binding(const core::object::texture_atlas_slot_t<AllocPolicy>& texture_slots);

	void render(const mesh_view_t& mesh_view);
	void render(const mesh_view_t& mesh_view, const core::render::camera_t& camera);

private:
	ShadingModel m_shading_model;
	renderer_t m_renderer;
};

template <class ShadingModel>
mesh_renderer_t(const ShadingModel&, const material_t&) -> mesh_renderer_t<ShadingModel>;

template <class ShadingModel>
mesh_renderer_t<ShadingModel>::mesh_renderer_t(ShadingModel model, const material_t& material) :
	m_shading_model(std::move(model)),
	m_renderer(model.renderer(material))
{}

template <class ShadingModel>
template <class T>
void mesh_renderer_t<ShadingModel>::set_uniform(T uniform_description_t::* uniform, T value) {
	m_renderer.set_uniform(uniform, value);
}

template <class ShadingModel>
void mesh_renderer_t<ShadingModel>::update_model_matrix(const glm::mat4& model_matrix) {
	m_renderer.set_uniform(&uniform_description_t::model_matrix, model_matrix);
}


template <class ShadingModel>
void mesh_renderer_t<ShadingModel>::update_projection(const glm::mat4& projection) {
	m_renderer.set_uniform(&uniform_description_t::view_projection, projection);
}

template <class ShadingModel>
void mesh_renderer_t<ShadingModel>::update_projection(const core::render::camera_t& camera) {
	update_projection(camera.mvp());
}

template <class ShadingModel>
template <class AllocPolicy>
void mesh_renderer_t<ShadingModel>::update_texture_binding(const core::object::texture_atlas_slot_t<AllocPolicy>& texture_slots) {
	m_shading_model.set_up(m_renderer, texture_slots);
}

template <class ShadingModel>
void mesh_renderer_t<ShadingModel>::render(const mesh_view_t& mesh_view) {
	m_renderer.render(mesh_view.view());
}

template <class ShadingModel>
void mesh_renderer_t<ShadingModel>::render(const mesh_view_t& mesh_view, const core::render::camera_t& camera) {
	update_model_matrix(mesh_view.model_matrix);
	update_projection(camera);
	render(mesh_view);
}

}