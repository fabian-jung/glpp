#pragma once

#define GLFW_INCLUDE_NONE

#include <string>

#include <glpp/glpp.hpp>
#include <GLFW/glfw3.h>
#include <map>
#include "input.hpp"

namespace glpp::system {

using opengl_version_t = std::pair<unsigned int, unsigned int>;
using mouse_position_t = std::pair<double, double>;

enum struct vsync_t : int {
	off = 0,
	on = 1
};

enum struct fullscreen_t : int {
	disabled = 0,
	enabled = 1
};

enum struct cursor_mode_t : int {
	normal = GLFW_CURSOR_NORMAL,
	hidden = GLFW_CURSOR_DISABLED,
	captured = GLFW_CURSOR_DISABLED
};

enum struct input_mode_t {
	contigious,
	wait
};

class window_t {
public:

	window_t(
		unsigned int width,
		unsigned int height,
		const std::string& name,
		vsync_t vsyncOn = vsync_t::on,
		fullscreen_t Fullscreen = fullscreen_t::disabled
	);

	~window_t();

	void set_cursor_mode(cursor_mode_t mode);
	cursor_mode_t get_cursor_mode();

	unsigned int get_width() const;
	unsigned int get_height() const;
	float get_aspect_ratio();
	const opengl_version_t& get_opengl_version();

	int get_key_state(int key);
	int get_mouse_button_state(int mouseButton);
	mouse_position_t get_mouse_pos();
	void set_mouse_pos(mouse_position_t pos);

	void clear(float r, float g, float b);
	void swap_buffer();

	template <class FN>
	void enter_main_loop(FN fn) {
		while(!should_close()) {
			glpp::call(glViewport, 0, 0, m_width, m_height);
			fn();
			swap_buffer();
			poll_events();
		}
	}

	void close();

	void set_input_mode(const input_mode_t& input_mode);
	input_mode_t get_input_mode() const;

	input_handler_t& input_handler() {
		return m_input_handler;
	}

private:

	unsigned int m_width, m_height;
	opengl_version_t m_version;
	std::string m_name;
	GLFWwindow* m_window;
	input_mode_t m_input_mode = input_mode_t::contigious;
	input_handler_t m_input_handler;

	static void glfw_error_callback(int error, const char* description);
	static void glfw_resize_window_callback(GLFWwindow* window, int width, int height);
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

	GLFWwindow* init_window(fullscreen_t fullscreen, vsync_t vsyncOn);
	bool should_close();
	void poll_events();

	cursor_mode_t cursor_mode;
};

} // End of namespace mapify::system


