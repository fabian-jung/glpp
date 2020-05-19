#include "glpp/system/input.hpp"
#include "glpp/system/window.hpp"
#include <iostream>
#include <bitset>

namespace glpp::system {

std::vector<joystick_t> input_handler_t::m_joysticks;

joystick_t::joystick_t(int id) :
	m_id(id)
{
	glfwGetJoystickAxes(id, &m_axis_count);
	glfwGetJoystickButtons(id, &m_button_count);
}
float joystick_t::axes(int axis) const {
	int _;
	return glfwGetJoystickAxes(m_id, &_)[axis];
}

unsigned char joystick_t::buttons(int btn) const {
	int _;
	return glfwGetJoystickButtons(m_id, &_)[btn];
}


int joystick_t::axes_count() const {
	return m_axis_count;
}

int joystick_t::button_count() const {
	return m_button_count;
}

int joystick_t::id() const {
	return m_id;
}

const char* joystick_t::name() const {
	return glfwGetJoystickName(m_id);
}

bool joystick_t::is_gamepad() const {
	return glfwJoystickIsGamepad(m_id);
}


input_handler_t::input_handler_t(window_t& window) {
	glfwSetFramebufferSizeCallback(window.m_window, &input_handler_t::glfw_resize_window_callback);
	glfwSetKeyCallback(window.m_window, &input_handler_t::glfw_key_callback);
	glfwSetCursorPosCallback(window.m_window, &input_handler_t::glfw_cursor_position_callback);
	glfwSetCursorEnterCallback(window.m_window, &input_handler_t::glfw_enter_callback);
	glfwSetMouseButtonCallback(window.m_window, &input_handler_t::glfw_mouse_button_callback);
	glfwSetScrollCallback(window.m_window, &input_handler_t::glfw_scroll_callback);

	for(auto i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
		if(glfwJoystickPresent(i)) {
			m_joysticks.emplace_back(i);
		}
	}
	glfwSetJoystickCallback(&input_handler_t::glfw_joystick_function);
}

void input_handler_t::glfw_error_callback(int error, const char* description) {
	std::cerr << "ERROR: GLFW returned error code " << std::bitset<sizeof(int)*4>(error) << std::endl;
	std::cerr << description << std::endl;
}

void input_handler_t::glfw_resize_window_callback(GLFWwindow* window, int width, int height) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->m_height = height;
	win->m_width = width;
	auto& window_resize_action = win->input_handler().m_window_resize_action;
	if(window_resize_action) {
		window_resize_action(width, height);
	}
}

void input_handler_t::glfw_cursor_position_callback(GLFWwindow* window, double x, double y) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_move_action = win->input_handler().m_mouse_move_action;
	if(mouse_move_action) {
		glm::vec2 mouse(
			(x/win->get_width())*2-1,
			(-y/win->get_height())*2+1
		);
		mouse_move_action(mouse, win->input_handler().m_mouse_pos);
		win->input_handler().m_mouse_pos = mouse;
	}
}

void input_handler_t::glfw_enter_callback(GLFWwindow* window, int entered) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	if(entered) {
		auto& mouse_enter_action = win->input_handler().m_mouse_enter_action;
		if(mouse_enter_action) {
			mouse_enter_action(win->get_mouse_pos());
		}
	} else {
		auto& mouse_leave_action = win->input_handler().m_mouse_leave_action;
		if(mouse_leave_action) {
			mouse_leave_action(win->input_handler().m_mouse_pos);
		}
	}
}

void input_handler_t::glfw_key_callback(GLFWwindow* window, int key, int, int action, int mods) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& keyboard_actions = win->input_handler().m_keyboard_actions;
	auto key_action_it = keyboard_actions.find({static_cast<key_t>(key), static_cast<action_t>(action)});
	if(key_action_it != keyboard_actions.end()) {
		if(key_action_it->second) {
			key_action_it->second.operator()(mods);
		}
	}
}

void input_handler_t::glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_actions = win->input_handler().m_mouse_button;
	auto mouse_action_it = mouse_actions.find({static_cast<mouse_button_t>(button), static_cast<action_t>(action)});
	if(mouse_action_it != mouse_actions.end()) {
		if(mouse_action_it->second) {
			double x, y;
			glfwGetCursorPos(window, &x, &y);
			glm::vec2 mouse(
				(x/win->get_width())*2-1,
				(-y/win->get_height())*2+1
			);
			mouse_action_it->second.operator()(mouse, mods);
		}
	}
}

void input_handler_t::glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	auto& mouse_scroll_action = win->input_handler().m_mouse_scroll_action;
	if(mouse_scroll_action) {
		mouse_scroll_action(glm::vec2(xoffset, yoffset));
	}
}

void input_handler_t::glfw_joystick_function(int id, int event) {
	if(event == GLFW_CONNECTED) {
		m_joysticks.emplace_back(id);
	}
	if(event == GLFW_DISCONNECTED) {
		auto it = std::find_if(m_joysticks.begin(), m_joysticks.end(), [id](const auto& joy){
			return joy.id() == id;
		});
		m_joysticks.erase(it);
	}
}

void input_handler_t::set_keyboard_action(key_t key, action_t event, key_action_callback_t callback) {
	m_keyboard_actions[{key, event}] = std::move(callback);
}

void input_handler_t::set_mouse_action(mouse_button_t key, action_t event, mouse_action_callback_t callback) {
	m_mouse_button[{key, event}] = std::move(callback);
}

void input_handler_t::set_mouse_move_action(mouse_move_action_t callback) {
	m_mouse_move_action = std::move(callback);
}

void input_handler_t::set_mouse_enter_action(mouse_enter_action_t callback) {
	m_mouse_enter_action = std::move(callback);
}

void input_handler_t::set_mouse_leave_action(mouse_leave_action_t callback) {
	m_mouse_leave_action = std::move(callback);
}

void input_handler_t::set_resize_action(glpp::system::input_handler_t::window_resize_action_t callback) {
	m_window_resize_action = std::move(callback);
}

void input_handler_t::set_mouse_scroll_action(glpp::system::input_handler_t::mouse_scroll_action_t callback)
{
	m_mouse_scroll_action = std::move(callback);
}

const std::vector<joystick_t>& input_handler_t::joysticks() const {
	return m_joysticks;
}

} // End of namespace glpp::system
