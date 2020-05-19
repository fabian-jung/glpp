#include "glpp/ui/ui.hpp"

namespace glpp::ui {

std::string ui_base_t::vertex_shader_code() const {
	return
		"#version 450 core\n\
		layout (location = 0) in vec2 pos;\n\
		layout (location = 1) in vec2 tex;\n\
		layout (location = 2) in int tex_id;\n\
		layout (location = 3) in vec4 tint;\n\
		out vec2 v_tex;\n\
		flat out int v_tex_id;\n\
		out vec4 v_tint;\n\
		\n\
		void main()\n\
		{\n\
			gl_Position = vec4(pos, 0, 1);\n\
			v_tex = tex;\n\
			v_tex_id = tex_id;\n\
			v_tint = tint;\n\
		}";
}

std::string ui_base_t::fragment_shader_code() const {
	std::string result =
	"#version 450 core\n";
	const auto number_of_textures = textures.size();
	if(number_of_textures>0) {
		result +=
		"uniform sampler2D textures["+std::to_string(number_of_textures)+"];\n";
	}
	result +=
	"flat in int v_tex_id;\n\
	in vec2 v_tex;\n\
	in vec4 v_tint;\n\
	out vec4 FragColor;\n\
	void main()\n\
	{\n";
	if(number_of_textures>0) {
		result +=
		"if(v_tex_id>=0) {\n\
		FragColor = v_tint*texture(textures[v_tex_id], v_tex);\n\
	} else {\n\
		FragColor = v_tint;\n\
	}\n";
	} else {
		result +=
		"FragColor = v_tint;\n";
	}
	result +=
	"}";
	return result;
}

}
