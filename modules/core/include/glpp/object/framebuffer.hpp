#pragma once

#include "glpp/glpp.hpp"
#include "glpp/object.hpp"
#include "texture.hpp"

namespace glpp::object {

enum class attachment_t : GLenum {
	color = GL_COLOR_ATTACHMENT0,
	depth = GL_DEPTH_ATTACHMENT,
	stencil = GL_STENCIL_ATTACHMENT,
	depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT
};

enum class framebuffer_target_t : GLenum {
	read = GL_READ_FRAMEBUFFER,
	write = GL_DRAW_FRAMEBUFFER,
	read_and_write = GL_FRAMEBUFFER
};

class framebuffer_t : public object_t<> {
public:

	framebuffer_t(size_t widht, size_t height);
	explicit framebuffer_t(std::initializer_list<std::pair<const texture_t&, attachment_t>> list);

	void attach(const texture_t& texture, attachment_t attatchment);
	void bind(framebuffer_target_t target = framebuffer_target_t::read_and_write);

	size_t width() const;
	size_t height() const;

	static void bind_default_framebuffer(framebuffer_target_t target = framebuffer_target_t::read_and_write);

private:
	GLuint create();
	static void destroy(GLuint id);

	size_t m_width;
	size_t m_height;
};

} // End of namespace glpp::object
