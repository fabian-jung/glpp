#pragma once

#include <string>

#include <glpp/gl.hpp>
#include <glm/glm.hpp>
#include "input.hpp"

class GLFWwindow;

namespace glpp::system {

using opengl_version_t = std::pair<unsigned int, unsigned int>;

enum struct vsync_t : int {
	off = 0,
	on = 1
};

enum struct fullscreen_t : int {
	disabled = 0,
	enabled = 1
};

enum struct cursor_mode_t : int {
	normal,
	hidden,
	captured
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

	window_t(window_t&& mov);
	window_t(const window_t& cpy) = delete;

	window_t& operator=(window_t&& mov);
	window_t& operator=(const window_t& cpy) = delete;

	~window_t();

	void set_cursor_mode(cursor_mode_t mode);
	cursor_mode_t get_cursor_mode();

	const std::string& get_name() const;

	unsigned int get_width() const;
	unsigned int get_height() const;
	glm::vec2 get_size() const;

	double get_dpi_x() const;
	double get_dpi_y() const;

	float get_aspect_ratio();
	void  set_aspect_ratio(float aspect);
	const opengl_version_t& get_opengl_version();

	int get_key_state(int key);
	int get_mouse_button_state(int mouseButton);
	glm::vec2 get_mouse_pos();
	void set_mouse_pos(glm::vec2 pos);

	void clear(float r, float g, float b);
	void swap_buffer();

	template <class FN>

	void enter_main_loop(FN fn) {
		while(!should_close()) {
			poll_events();
			glViewport(0, 0, m_width, m_height);
			fn();
			swap_buffer();
		}
	}

	void close();

	void set_input_mode(const input_mode_t& input_mode);
	input_mode_t get_input_mode() const;

	input_handler_t& input_handler() {
		return m_input_handler;
	}

	bool should_close();
	void poll_events();

private:
	friend input_handler_t;
	friend void glfw_resize_window_callback(GLFWwindow* window, int width, int height);

	GLFWwindow* init_window(fullscreen_t fullscreen, vsync_t vsyncOn);

	unsigned int m_width, m_height;
	opengl_version_t m_version;
	std::string m_name;
	GLFWwindow* m_window;
	input_mode_t m_input_mode = input_mode_t::contigious;
	cursor_mode_t cursor_mode;
	input_handler_t m_input_handler;
};

} // End of namespace mapify::system


