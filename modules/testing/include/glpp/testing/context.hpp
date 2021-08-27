#pragma once

#include <glpp/core/object/image.hpp>
#include <glpp/core/object/framebuffer.hpp>
#include "offscreen_driver.hpp"
#include "window_driver.hpp"

namespace glpp::test {

template <class Driver>
class context_t {
public:

    context_t(const size_t width, const size_t height, Driver driver) :
        m_driver(std::move(driver)),
        m_framebuffer(width, height),
        m_texture(width, height, core::object::image_format_t::rgb_8)
    {
        m_framebuffer.attach(m_texture, core::object::attachment_t::color);
        m_framebuffer.bind();
        glViewport(0, 0, width, height);
    };

    context_t(const size_t width, const size_t height) :
        context_t(width, height, Driver{})
    {}

    context_t(context_t&& mov) = default;
    context_t(const context_t& cpy) = default;

    context_t& operator=(context_t&& mov) = delete;
    context_t& operator=(const context_t& cpy) = delete;

    template <class Color = glm::vec3>
    core::object::image_t<Color> swap_buffer() {
        return m_framebuffer.pixel_read<Color>();
    }

private:
    Driver m_driver;
    core::object::framebuffer_t m_framebuffer;
    core::object::texture_t m_texture;
};

template <class Driver>
context_t(const size_t, const size_t, Driver) -> context_t<Driver>;

extern template class context_t<window_driver_t>;
extern template class context_t<offscreen_driver_t>;

}