#include <glpp/testing/offscreen_driver.hpp>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cassert>
#include <chrono>

#include <glpp/gl.hpp>
#include <glpp/gl/context.hpp>

namespace glpp::test {

void assertEGLError(const std::string& msg) {
	EGLint error = eglGetError();

	if (error != EGL_SUCCESS) {
		std::stringstream s;
		s << "EGL error 0x" << std::hex << error << " at " << msg;
		throw std::runtime_error(s.str());
	}
}

offscreen_driver_t::offscreen_driver_t(const driver_t driver) {
    /* get an EGL display connection */
    EGLint error = EGL_SUCCESS;
    using clock = std::chrono::high_resolution_clock;
    const auto begin = clock::now();
    std::chrono::duration<double> elapsed_time { 0 };
    {
        if(error != EGL_SUCCESS) {
            std::cout << "EGL surface creation failed with error 0x" << std::hex << error << std::dec << ". Force restart. Elapsed time so far: " << elapsed_time.count() <<"s." << std::endl;
        }

        if(driver == driver_t::native) {
            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        } else {
            display = eglGetPlatformDisplay(EGL_PLATFORM_SURFACELESS_MESA, nullptr, nullptr);
        }

        assertEGLError("get display");
        /* initialize the EGL display connection */
        eglInitialize(display, NULL, NULL);
        error = eglGetError();
        const auto current = clock::now();
        elapsed_time = current-begin;
        // eglInitialize seems to be flaky with mesa
        // try to force context creation for 30s and then timeout
    } while (error != EGL_SUCCESS && elapsed_time.count() < 30.0); 
    assertEGLError("initialize");

    constexpr std::array<EGLint, 13> context_attrib {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 5,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_FALSE,
        EGL_CONTEXT_OPENGL_DEBUG, EGL_FALSE,
        EGL_NONE
    };

    constexpr std::array<EGLint, 13> config_Attrib {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    /* get an appropriate EGL frame buffer configuration */
    eglChooseConfig(display, config_Attrib.data(), &config, 1, &num_config);
    assertEGLError("choose config");
    
    eglBindAPI(EGL_OPENGL_API);
    assertEGLError("bind opengl api");

    /* create an EGL rendering context */
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, context_attrib.data());
    assertEGLError("create context");
    
    /* connect the context to the surface */
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, context);
    assertEGLError("make context current");
    
    glpp::init(&eglGetProcAddress);

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    std::cout << "Generate EGL context with version " << major << "." << minor << std::endl;
    std::cout << glGetString( GL_VERSION ) << std::endl;
    std::cout << glGetString( GL_VENDOR ) << std::endl;
    std::cout << glGetString( GL_RENDERER ) << std::endl;
    std::cout << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

    assert(major >= 4);
    assert(minor >= 5);
}

offscreen_driver_t::~offscreen_driver_t() {
    eglDestroyContext(display, context);
    assertEGLError("destroy context");
    
    eglTerminate(display);
    assertEGLError("terminate");
}

}