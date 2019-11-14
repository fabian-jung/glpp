#include "glpp/system/window.hpp"

#include <exception>
#include <functional>
#include <bitset>
#include <iostream>

// #include "opengl/call.hpp"

namespace glpp::system {

// std::map<window_t*, input_handler_t> window_t::input_handler_map;

template <class FN, class... ARGS>
auto call(FN fn, ARGS&& ...args) {
	return fn(std::forward<ARGS>(args)...);
}

window_t::window_t(unsigned int width, unsigned int height, const std::string& name, vsync_t vsyncOn, fullscreen_t fullScreen) :
	width(width),
	height(height),
	version(opengl_version_t(4,5)),
	name(name),
	window(init_window(fullScreen, vsyncOn)),
	cursor_mode(cursor_mode_t::normal)
{
	if (window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Could not create window.");
	}

	//Check GL context
	GLint glMajor = 0;
	GLint glMinor = 0;
	call(glGetIntegerv, GL_MAJOR_VERSION, &glMajor);
	call(glGetIntegerv, GL_MINOR_VERSION, &glMinor);
	std::cout << "GL " << glMajor << "." << glMinor << " context created." << std::endl;

	version = opengl_version_t(glMajor,glMinor);

	const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString (GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// Set up Gl Stuff

	call(glClearColor, .0f,.0f,.0f,1.000f);
	call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);


	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
	//supress first error after init. it seems to have no reason.
	glGetError();

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	call(glDepthFunc, GL_LESS); // depth-testing interprets a smaller value as "closer"
	// enable depth-testing
	call(glEnable, GL_DEPTH_TEST);
	// Enable Antialiasing by multisampling
	call(glEnable, GL_MULTISAMPLE);

	// Texture input aligned bytewise // this is the slowest, but safest way to upload textures
	call(glPixelStorei, GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction


}
window_t::~window_t()
{
	std::cout << "window_t destroy" << std::endl;

	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* window_t::init_window(fullscreen_t fullScreen, vsync_t vsync) {
	// start GL context and OS window using the GLFW helper library
	static bool init_successful = glfwInit();
	if (!init_successful) {
		std::cerr << "ERROR: could not start GLFW3" << std::endl;
		throw std::runtime_error("Could not initialise GLFW");
	}

	glfwSetErrorCallback(&window_t::glfw_error_callback);

	//glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint (GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, version.first);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, version.second);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //Default value
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Better compatability
// 	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	//Anti Aliasing
	glfwWindowHint (GLFW_SAMPLES, 4);

	//GL Profile
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* w;
	if(fullScreen == fullscreen_t::enabled) {
		w = glfwCreateWindow (width, height, name.c_str(), glfwGetPrimaryMonitor(), NULL);
	} else {
		w = glfwCreateWindow (width, height, name.c_str(), NULL, NULL);
	}
	glfwMakeContextCurrent (w);

	glfwSetWindowUserPointer(w, this);
	glfwSwapInterval(static_cast<int>(vsync));

	glfwSetFramebufferSizeCallback(w, &window_t::glfw_resize_window_callback);
	glfwSetKeyCallback(w, &window_t::glfw_key_callback);
	glfwSetCursorPosCallback(w, &window_t::glfw_cursor_position_callback);
	glfwSetMouseButtonCallback(w, &window_t::glfw_mouse_button_callback);
	glfwSetScrollCallback(w, &window_t::glfw_scroll_callback);

	return w;
}

void window_t::set_cursor_mode(cursor_mode_t mode)
{
	cursor_mode = mode;
	glfwSetInputMode(window, GLFW_CURSOR, static_cast<int>(mode));
}

window_t::cursor_mode_t window_t::get_cursor_mode() {
	return cursor_mode;
}


// input_handler_t& window_t::get_input_handler() {
// 	return input_handler_map[this];
// }

unsigned int window_t::get_width() const {
	return width;
}

unsigned int window_t::get_height() const {
	return height;
}

float window_t::get_aspect_ratio()
{
	return static_cast<float>(width)/static_cast<float>(height);
}

const opengl_version_t& window_t::get_opengl_version() {
	return version;
}

int window_t::get_mouse_button_state(int mouseButton)
{
	return glfwGetMouseButton(window, mouseButton);
}

int window_t::get_key_state(int key)
{
	return glfwGetKey(window, key);
}

mouse_position_t window_t::get_mouse_pos()
{
	mouse_position_t result;
	glfwGetCursorPos(window, &result.first, &result.second);
	result.first /=  width;
	result.second /= height;
	return result;
}

void window_t::set_mouse_pos(mouse_position_t pos) {
	glfwSetCursorPos(window, pos.first, pos.second);
}

void window_t::clear(float r, float g, float b)
{
	glClearColor(r,g,b,1.000f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_t::swap_buffer() {
	glfwSwapBuffers(window);
}

bool window_t::should_close() {
// 	std::cout << window << " should close = " << glfwWindowShouldClose(window) << std::endl;
	return glfwWindowShouldClose(window);
}

void window_t::poll_events() {
	glfwPollEvents();
}

void window_t::glfw_error_callback(int error, const char* description) {
	std::cerr << "ERROR: GLFW returned error code " << std::bitset<sizeof(int)*4>(error) << std::endl;
	std::cerr << description << std::endl;
}

void window_t::glfw_resize_window_callback(GLFWwindow* window, int width, int height) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->height = height;
	win->width = width;
}

void window_t::glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
// 	auto input_handler_it = input_handler_map.find(win);
// 	if(input_handler_it != input_handler_map.end()) {
// 		auto& input_handler = input_handler_it->second;
// 		input_handler_t::event_t event = {
// 			input_handler_t::event_type_t::mouse_move,
// 			.mouse_move = { xpos / win->width,  ypos / win->height }
// 		};
//
// 		input_handler.event_queue.push(std::move(event));
// 	}
}
void window_t::glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
// 	auto input_handler_it = input_handler_map.find(win);
// 	if(input_handler_it != input_handler_map.end()) {
// 		auto& input_handler = input_handler_it->second;
// 		input_handler_t::event_t event {
// 			input_handler_t::event_type_t::key,
// 			.key_action = {
// 				static_cast<input_handler_t::key_t>(key),
// 				scancode,
// 				static_cast<input_handler_t::action_t>(action),
// 				mods
// 			}
// 		};
//
// 		input_handler.event_queue.push(std::move(event));
// 	}
}

void window_t::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
// 	auto input_handler_it = input_handler_map.find(win);
// 	if(input_handler_it != input_handler_map.end()) {
// 		auto& input_handler = input_handler_it->second;
// 		input_handler_t::event_t event {
// 			input_handler_t::event_type_t::mouse_button,
// 			.mouse_action = {
// 				static_cast<input_handler_t::mouse_button_t>(button),
// 				static_cast<input_handler_t::action_t>(action),
// 				mods
// 			}
// 		};
//
// 		input_handler.event_queue.push(std::move(event));
// 	}
}

void window_t::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
// 	auto input_handler_it = input_handler_map.find(win);
// 	if(input_handler_it != input_handler_map.end()) {
// 		auto& input_handler = input_handler_it->second;
// 		input_handler_t::event_t event {
// 			input_handler_t::event_type_t::mouse_scroll,
// 			.mouse_scroll = {
// 				xoffset,
// 				yoffset
// 			}
// 		};
//
// 		input_handler.event_queue.push(std::move(event));
// 	}
}

void window_t::close() {
	glfwSetWindowShouldClose(window, true);
}

} // End of namespace system

