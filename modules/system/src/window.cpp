#include "glpp/system/window.hpp"

#include <exception>
#include <functional>
#include <bitset>
#include <iostream>
#include <string_view>
#include <sstream>
#include <glpp/gl/context.hpp>
namespace glpp::system {

void error_handler(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	auto window = reinterpret_cast<const window_t*>(userParam);
	std::stringstream msg;
	msg << "\n";

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             msg << "Source: API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   msg << "Source: Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: msg << "Source: Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:     msg << "Source: Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION:     msg << "Source: Application"; break;
		case GL_DEBUG_SOURCE_OTHER:           msg << "Source: Other"; break;
	}
	msg << "\n";


	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               msg << "Type: Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: msg << "Type: Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  msg << "Type: Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY:         msg << "Type: Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE:         msg << "Type: Performance"; break;
		case GL_DEBUG_TYPE_MARKER:              msg << "Type: Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP:          msg << "Type: Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP:           msg << "Type: Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER:               msg << "Type: Other"; break;
	}
	msg << "\n";

	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         msg << "Severity: high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM:       msg << "Severity: medium"; break;
		case GL_DEBUG_SEVERITY_LOW:          msg << "Severity: low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: msg << "Severity: notification"; break;
	}
	msg << "\n";

	msg << "Debug message (" << id << ") from window " << window->get_name() <<  ": " <<  std::string_view(message, length) << "\n";


	throw std::runtime_error(msg.str());
}

window_t::window_t(unsigned int width, unsigned int height, const std::string& name, vsync_t vsyncOn, fullscreen_t fullScreen) :
	m_width(width),
	m_height(height),
	m_version(opengl_version_t(4,5)),
	m_name(name),
	m_window(init_window(fullScreen, vsyncOn)),
	cursor_mode(cursor_mode_t::normal),
	m_input_handler(*this)
{
	if (m_window == nullptr) {
		glfwTerminate();
		throw std::runtime_error("Could not create window.");
	}

	// load dynamic opengl functions
	glpp::init(&glfwGetProcAddress);
	
	//Check GL context
	GLint glMajor = 0;
	GLint glMinor = 0;
	glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
	glGetIntegerv(GL_MINOR_VERSION, &glMinor);
	std::cout << "GL " << glMajor << "." << glMinor << " context created." << std::endl;

	m_version = opengl_version_t(glMajor,glMinor);

	const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString (GL_VERSION); // version as a string
	std::cout << "Renderer: " << renderer << std::endl;
	std::cout << "OpenGL version supported " << version << std::endl;

	// Set up Gl Stuff
#ifndef NDEBUG
	glDebugMessageCallback(error_handler, this);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, true);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_MEDIUM, 0, nullptr, true);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, true);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
	constexpr std::array ignore_buffer {
		static_cast<GLuint>(131218)
	};
	glDebugMessageControl(GL_DEBUG_SOURCE_API,  GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE, ignore_buffer.size(), ignore_buffer.data(), false);

#endif

	glClearColor(.0f,.0f,.0f,1.000f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(m_window);


	//supress first error after init. it seems to have no reason.
	glGetError();

	// Initialise error callback function
	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	// enable depth-testing
	glEnable(GL_DEPTH_TEST);
	// Enable Antialiasing by multisampling
	glEnable(GL_MULTISAMPLE);

	// Texture input aligned bytewise // this is the slowest, but safest way to upload textures
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

}

window_t::window_t(window_t&& mov) :
	m_width{mov.m_width},
	m_height{mov.m_height},
	m_version{mov.m_version},
	m_name{std::move(mov.m_name)},
	m_window{mov.m_window},
	m_input_mode{mov.m_input_mode},
	cursor_mode{mov.cursor_mode},
	m_input_handler{std::move(mov.m_input_handler)}
{
	mov.m_window = nullptr;
}

window_t::~window_t()
{
	if(m_window != nullptr) {
		glfwDestroyWindow(m_window);
	}
}

struct glfw_context_t {
	~glfw_context_t() {
		glfwTerminate();
	}

	bool init_successful = glfwInit();
};

GLFWwindow* window_t::init_window(fullscreen_t fullScreen, vsync_t vsync) {
	// start GL context and OS window using the GLFW helper library
	static glfw_context_t glfw_context;
	if (!glfw_context.init_successful) {
		throw std::runtime_error("Could not initialise GLFW");
	}

	glfwSetErrorCallback(&input_handler_t::glfw_error_callback);

	//glfwWindowHint (GLFW_CLIENT_API, GLFW_OPENGL_API);
	//glfwWindowHint (GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, m_version.first);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, m_version.second);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE); //Default value
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //Better compatability

#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
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

	return w;
}

void window_t::set_cursor_mode(cursor_mode_t mode)
{
	cursor_mode = mode;
	glfwSetInputMode(m_window, GLFW_CURSOR, static_cast<int>(mode));
}

const std::string& window_t::get_name() const {
	return m_name;
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

glm::vec2 window_t::get_size() const {
	return {m_width, m_height};
}

float window_t::get_aspect_ratio()
{
	return static_cast<float>(m_width)/static_cast<float>(m_height);
}

void window_t::set_aspect_ratio(float aspect)
{
	glfwSetWindowAspectRatio(m_window, aspect*1000, 1000);
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

glm::vec2 window_t::get_mouse_pos()
{
	double x, y;
	glfwGetCursorPos(m_window, &x, &y);
	glm::vec2 mouse(
		(x/get_width())*2-1,
		(-y/get_height())*2+1
	);
	return mouse;
}

void window_t::set_mouse_pos(glm::vec2 pos) {
	pos = 0.5f*glm::vec2(get_width(), -get_height())*(pos+glm::vec2(1.0f,-1.0f));
	glfwSetCursorPos(m_window, pos.x, pos.y);
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

