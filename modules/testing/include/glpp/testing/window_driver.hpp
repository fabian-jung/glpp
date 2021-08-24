#pragma once

#include <glpp/system/window.hpp>

namespace glpp::test {

class window_driver_t {
public:
    window_driver_t() :
        m_window(1, 1, "test context window", system::vsync_t::off)
    {}

private:
    system::window_t m_window;
};

}