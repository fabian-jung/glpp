#pragma once

#include <glm/glm.hpp>
#include "glpp/core/object/texture.hpp"

namespace glpp::core::object::texture_atlas
{
    
struct grid_policy_t {
	using key_t = glm::uvec2;
	class storage_t {
		explicit storage_t(glm::vec2 size);
		storage_t(size_t width, size_t height, object::image_format_t format) :
			m_texture(width, height, format)
		{}

		private:
			glpp::core::object::texture_t m_texture;
	};
};

} // namespace name
