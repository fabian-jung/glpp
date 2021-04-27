#include "glpp/core/object/framebuffer.hpp"

namespace glpp::core::object {

framebuffer_t::framebuffer_t(size_t width, size_t height) :
	object_t(
		create(),
		destroy
	),
	m_width(width),
	m_height(height)
{}

framebuffer_t::framebuffer_t(std::initializer_list<std::pair<const texture_t&, attachment_t>> list) :
	object_t(
		create(),
		destroy
	),
	m_width(list.begin()->first.width()),
	m_height(list.begin()->first.height())
{
	for(const auto& [texture, attachment] : list) {
		attach(texture, attachment);
	}
}

GLuint framebuffer_t::create() {
	GLuint id;
	glCreateFramebuffers(1, &id);
	return id;
}

void framebuffer_t::destroy(GLuint id) {
	glDeleteFramebuffers(1, &id);
}

void framebuffer_t::attach(const texture_t& texture, attachment_t attatchment) {
	if(width() != texture.width() || height() != texture.height()) {
		throw std::runtime_error("Can not attach texture with different resolution.");
	}
	glNamedFramebufferTexture(id(), static_cast<GLenum>(attatchment), texture.id(), 0);
}

void framebuffer_t::bind(framebuffer_target_t target) {
	if(glCheckNamedFramebufferStatus(id(), GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Framebuffer is not complete and can not be bound. Attach at least one buffer before binding.");
	}
	glBindFramebuffer(static_cast<GLenum>(target), id());
}

void framebuffer_t::bind_default_framebuffer(framebuffer_target_t target) {
	glBindFramebuffer(static_cast<GLenum>(target), 0);
}

size_t framebuffer_t::width() const {
	return m_width;
}

size_t framebuffer_t::height() const {
	return m_height;
}

}
