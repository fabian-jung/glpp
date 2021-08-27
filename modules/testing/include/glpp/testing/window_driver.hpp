#pragma once

#include <glpp/system/window.hpp>

namespace glpp::test {

struct window_driver_t {
    system::window_t m_window {1, 1, "test context window", system::vsync_t::off};
};

}