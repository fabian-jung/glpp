#include "glpp/asset/shading/texture_storage.hpp"

namespace glpp::asset::shading {


object::texture_t& texture_storage_t::texture(const std::string& file) {
	return get_storage_element(file).texture;
}

const object::texture_slot_t& texture_storage_t::slot(const std::string& file) {
	return get_storage_element(file).slot;
}


texture_storage_t::texture_storage_element_t& texture_storage_t::get_storage_element(const std::string& file) {
	auto it = m_textures.find(file);
	if(it != m_textures.end()) {
		return it->second;
	}

	object::texture_t tex {
		object::image_t<glm::vec3>{file.c_str()}
	};
	auto slot = tex.bind_to_texture_slot();
	auto [nit, success] = m_textures.emplace(
		file,
		texture_storage_element_t {
			std::move(tex),
			std::move(slot)
		}
	);
	if(success) {
		return nit->second;
	}
	throw std::runtime_error("Could not create texture for texture_storage_t.");
}

}
