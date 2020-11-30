#pragma once

#include "scene_view.hpp"
#include "mesh_renderer.hpp"

namespace glpp::asset::render {

template<class ShadingModel>
class scene_renderer_t {
public:
	using material_key_t = size_t;
	using renderer_t = mesh_renderer_t<ShadingModel>;

	template <class... Args>
	explicit scene_renderer_t(const scene_t& scene, Args&&... args);

	void render(const scene_view_t& view);
	void render(const scene_view_t& view, const glpp::core::render::camera_t& camera);
	renderer_t& renderer(material_key_t index);
	const renderer_t& renderer(material_key_t index) const;

private:
	std::vector<renderer_t> m_renderers;
};

template<class ShadingModel>
template <class... Args>
scene_renderer_t<ShadingModel>::scene_renderer_t(const scene_t& scene, Args&&... args)
{
	std::transform(
		scene.materials.begin(),
		scene.materials.end(),
		std::back_inserter(m_renderers),
		[&](const material_t& material) {
			return mesh_renderer_t<ShadingModel>{ material, std::forward<Args>(args)... };
		}
	);

}

template<class ShadingModel>
void scene_renderer_t<ShadingModel>::render(const scene_view_t& view) {
	for(auto i = 0u; i < m_renderers.size(); ++i) {
		const auto& meshes = view.meshes_by_material(i);
		auto& renderer = m_renderers[i];
		for(const auto& mesh : meshes) {
			renderer.render(mesh);
		}
	}
}

template<class ShadingModel>
void scene_renderer_t<ShadingModel>::render(const scene_view_t& view, const glpp::core::render::camera_t& camera) {
	for(auto i = 0u; i < m_renderers.size(); ++i) {
		const auto& meshes = view.meshes_by_material(i);
		auto& renderer = m_renderers[i];
		for(const auto& mesh : meshes) {
			renderer.render(mesh, camera);
		}
	}
}

template<class ShadingModel>
typename scene_renderer_t<ShadingModel>::renderer_t& scene_renderer_t<ShadingModel>::renderer(material_key_t index) {
	return m_renderers[index];
}

template<class ShadingModel>
const typename scene_renderer_t<ShadingModel>::renderer_t& scene_renderer_t<ShadingModel>::renderer(material_key_t index) const {
	return m_renderers[index];
}

}
