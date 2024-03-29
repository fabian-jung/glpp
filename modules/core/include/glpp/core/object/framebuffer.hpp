#pragma once

#include "glpp/gl.hpp"
#include "glpp/core/object.hpp"
#include "texture.hpp"

namespace glpp::core::object {

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

	framebuffer_t(size_t width, size_t height);
	explicit framebuffer_t(std::initializer_list<std::pair<const texture_t&, attachment_t>> list);

	framebuffer_t(const framebuffer_t& cpy) = delete;
	framebuffer_t(framebuffer_t&& mov) = default;

	framebuffer_t& operator=(const framebuffer_t& cpy) = delete;
	framebuffer_t& operator=(framebuffer_t&& mov) = default;

	void attach(const texture_t& texture, attachment_t attatchment);
	void bind(framebuffer_target_t target = framebuffer_target_t::read_and_write);

	size_t width() const;
	size_t height() const;

	static void bind_default_framebuffer(framebuffer_target_t target = framebuffer_target_t::read_and_write);
	static framebuffer_t get_default_framebuffer();

	template <class T = glm::vec3>
	image_t<T> pixel_read(size_t x = 0, size_t y = 0) const;

	template <class T = glm::vec3>
	image_t<T> pixel_read(size_t x, size_t y, size_t width, size_t height) const;

private:
	framebuffer_t();
	
	GLuint create();
	static void destroy(GLuint id);

	size_t m_width;
	size_t m_height;
};

template <class T>
image_t<T> framebuffer_t::pixel_read(size_t x, size_t y) const {
	return pixel_read<T>(x, y, m_width - x, m_height - y);
}

template <class T>
image_t<T> framebuffer_t::pixel_read(size_t x, size_t y, size_t width, size_t height) const {
	image_t<T> result(width, height);
	glNamedFramebufferReadBuffer(id(), GL_COLOR_ATTACHMENT0);
	glReadPixels(x, y, width, height, static_cast<GLenum>(result.format()), result.type(), result.data());
	return result;
}

} // End of namespace glpp::object
