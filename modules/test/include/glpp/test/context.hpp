#pragma once

#include <glpp/object/image.hpp>
#include <glpp/object/framebuffer.hpp>
#include "window_driver.hpp"

namespace glpp::test {

template <class Driver>
class context_t {
public:

    context_t(const size_t width, const size_t height) :
        m_texture(width, height, object::image_format_t::rgb_8),
        m_framebuffer(width, height)
    {
        m_framebuffer.attach(m_texture, object::attachment_t::color);
        m_framebuffer.bind();
        glViewport(0, 0, width, height);
    };

    context_t(context_t&& mov) = default;
    context_t(const context_t& cpy) = default;

    context_t& operator=(context_t&& mov) = delete;
    context_t& operator=(const context_t& cpy) = delete;

    template <class Color = glm::vec3>
    object::image_t<Color> swap_buffer() {
        return m_framebuffer.pixel_read<Color>();
    }

private:
    Driver m_driver;
    object::texture_t m_texture;
    object::framebuffer_t m_framebuffer;
};

extern template class context_t<window_driver_t>;

}