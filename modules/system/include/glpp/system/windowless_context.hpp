#pragma once
#define GLEW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace glpp::system {

enum class driver_t {
    native,
    mesa
};

class windowless_context_t {
public:
    
    windowless_context_t(const driver_t driver = driver_t::mesa);

    windowless_context_t(const windowless_context_t& cpy) = delete;
    windowless_context_t(windowless_context_t&& mov);
    
    windowless_context_t& operator=(const windowless_context_t& cpy) = delete;
    windowless_context_t& operator=(windowless_context_t&& mov);
    
    ~windowless_context_t();

private:
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;    
    EGLint num_config;
};

}