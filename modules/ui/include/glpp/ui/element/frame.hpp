#pragma once

#include "glpp/ui/ui.hpp"

namespace glpp::ui::element {

	struct frame_t {
		glm::vec4 tint {1.0f};

		void triangulate(
			ui_base_t::model_t& model,
			const glm::vec2 bottom_left,
			const glm::vec2 top_right,
			const ui_base_t::textures_t& textures
		);

		void textures(ui_base_t::textures_t&);

		template <class Eve, class... Args>
		void act(Eve, const Args&...) {}
	};

}

