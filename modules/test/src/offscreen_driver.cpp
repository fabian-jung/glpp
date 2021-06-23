#include <glpp/test/offscreen_driver.hpp>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <cassert>

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

offscreen_driver_t::offscreen_driver_t() {
    /* get an EGL display connection */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    assertEGLError("get display");

    /* initialize the EGL display connection */
    eglInitialize(display, NULL, NULL);
    assertEGLError("initialize");
    
    const std::vector<EGLint> attrib {
         EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 5,
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE, EGL_FALSE,
        EGL_CONTEXT_OPENGL_DEBUG, EGL_FALSE,
        EGL_NONE
    };

    /* get an appropriate EGL frame buffer configuration */
    eglChooseConfig(display, nullptr, &config, 1, &num_config);
    assertEGLError("choose config");
    
    eglBindAPI(EGL_OPENGL_API);
    assertEGLError("bind opengl api");

    /* create an EGL rendering context */
    

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib.data());
    assertEGLError("create context");
    

    // /* create an EGL window surface */
    // surface = eglCreateWindowSurface(display, config, NULL, NULL);

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

    // assert(major >= 4);
    // assert(minor >= 5);

    GLuint id;
    glCreateFramebuffers(1, &id);
    /* clear the color buffer */

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    // eglSwapBuffers(display, surface);
}

offscreen_driver_t::~offscreen_driver_t() {
    eglDestroyContext(display, context);
    assertEGLError("destroy context");
    
    eglTerminate(display);
    assertEGLError("terminate");
}

}