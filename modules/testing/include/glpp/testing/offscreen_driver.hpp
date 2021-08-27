#pragma once
#include <glpp/system/windowless_context.hpp>
namespace glpp::test {

struct offscreen_driver_t {
    system::windowless_context_t m_context { system::driver_t::mesa };
};

}