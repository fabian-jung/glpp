#pragma once
#define GLEW_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>

namespace glpp::test {

class offscreen_driver_t {
public:
    
    offscreen_driver_t();
    ~offscreen_driver_t();

private:
    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    EGLint num_config;
};

}