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
	m_width(width),
	m_height(height),
	m_version(opengl_version_t(4,5)),
	m_name(name),
	m_window(init_window(fullScreen, vsyncOn)),
	cursor_mode(cursor_mode_t::normal)
{
	if (m_window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Could not create window.");
	}

	//Check GL context
	GLint glMajor = 0;
	GLint glMinor = 0;
	call(glGetIntegerv, GL_MAJOR_VERSION, &glMajor);
	call(glGetIntegerv, GL_MINOR_VERSION, &glMinor);
	std::cout << "GL " << glMajor << "." << glMinor << " context created." << std::endl;

	m_version = opengl_version_t(glMajor,glMinor);

	const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString (GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) {
		throw std::runtime_error("OpenGL functions could not be loaded.");
	};

	// Set up Gl Stuff
	call(glClearColor, .0f,.0f,.0f,1.000f);
	call(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(m_window);


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

	glfwDestroyWindow(m_window);
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
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, m_version.first);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, m_version.second);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //Default value
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Better compatability
// 	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	//Anti Aliasing
	glfwWindowHint (GLFW_SAMPLES, 4);

	//GL Profile
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* w;
	if(fullScreen == fullscreen_t::enabled) {
		w = glfwCreateWindow (m_width, m_height, m_name.c_str(), glfwGetPrimaryMonitor(), NULL);
	} else {
		w = glfwCreateWindow (m_width, m_height, m_name.c_str(), NULL, NULL);
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
	glfwSetInputMode(m_window, GLFW_CURSOR, static_cast<int>(mode));
}

cursor_mode_t window_t::get_cursor_mode() {
	return cursor_mode;
}

unsigned int window_t::get_width() const {
	return m_width;
}

unsigned int window_t::get_height() const {
	return m_height;
}

float window_t::get_aspect_ratio()
{
	return static_cast<float>(m_width)/static_cast<float>(m_height);
}

const opengl_version_t& window_t::get_opengl_version() {
	return m_version;
}

int window_t::get_mouse_button_state(int mouseButton)
{
	return glfwGetMouseButton(m_window, mouseButton);
}

int window_t::get_key_state(int key)
{
	return glfwGetKey(m_window, key);
}

mouse_position_t window_t::get_mouse_pos()
{
	mouse_position_t result;
	glfwGetCursorPos(m_window, &result.first, &result.second);
	result.first /=  m_width;
	result.second /= m_height;
	return result;
}

void window_t::set_mouse_pos(mouse_position_t pos) {
	glfwSetCursorPos(m_window, pos.first, pos.second);
}

void window_t::clear(float r, float g, float b)
{
	glClearColor(r,g,b,1.000f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void window_t::swap_buffer() {
	glfwSwapBuffers(m_window);
}

bool window_t::should_close() {
	return glfwWindowShouldClose(m_window);
}

void window_t::poll_events() {
	switch(m_input_mode) {
		case input_mode_t::contigious:
			glfwPollEvents();
			break;
		case input_mode_t::wait:
			glfwWaitEvents();
			break;
	}
}

void window_t::glfw_error_callback(int error, const char* description) {
	std::cerr << "ERROR: GLFW returned error code " << std::bitset<sizeof(int)*4>(error) << std::endl;
	std::cerr << description << std::endl;
}

void window_t::glfw_resize_window_callback(GLFWwindow* window, int width, int height) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->m_height = height;
	win->m_width = width;
	auto& window_resize_action = win->m_input_handler.m_window_resize_action;
	if(window_resize_action) {
		window_resize_action(width, height);
	}
}

void window_t::glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_move_action = win->m_input_handler.m_mouse_move_action;
	if(mouse_move_action) {
		mouse_move_action(xpos, ypos);
	}
}

void window_t::glfw_key_callback(GLFWwindow* window, int key, int, int action, int mods) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& keyboard_actions = win->m_input_handler.m_keyboard_actions;
	auto key_action_it = keyboard_actions.find({static_cast<key_t>(key), static_cast<action_t>(action)});
	if(key_action_it != keyboard_actions.end()) {
		if(key_action_it->second) {
			key_action_it->second.operator()(mods);
		}
	}
}

void window_t::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
 	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_actions = win->m_input_handler.m_mouse_button;
	auto mouse_action_it = mouse_actions.find({static_cast<mouse_button_t>(button), static_cast<action_t>(action)});
	if(mouse_action_it != mouse_actions.end()) {
		if(mouse_action_it->second) {
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			mouse_action_it->second.operator()(xpos, ypos, mods);
		}
	}
}

void window_t::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_scroll_action = win->m_input_handler.m_mouse_scroll_action;
	if(mouse_scroll_action) {
		mouse_scroll_action(xoffset, yoffset);
	}
}

void window_t::close() {
	glfwSetWindowShouldClose(m_window, true);
}

void window_t::set_input_mode(const input_mode_t& input_mode) {
	m_input_mode = input_mode;
}

input_mode_t window_t::get_input_mode() const {
	return m_input_mode;
}

double window_t::get_dpi_x() const {
	int width, height;
	auto monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorPhysicalSize(monitor, &width, &height);
	constexpr double mm_to_inch = 0.0393701;
	return static_cast<double>(get_width()) / (static_cast<double>(width)*mm_to_inch);
}

double window_t::get_dpi_y() const {
	int width, height;
	auto monitor = glfwGetPrimaryMonitor();
	glfwGetMonitorPhysicalSize(monitor, &width, &height);
	constexpr double mm_to_inch = 0.0393701;
	return static_cast<double>(get_width()) / (static_cast<double>(width)*mm_to_inch);
}


} // End of namespace system

