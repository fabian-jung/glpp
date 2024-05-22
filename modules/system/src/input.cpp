#include "glpp/system/input.hpp"
#include "glpp/system/window.hpp"
#include <compare>
#include <iostream>
#include <set>
#include <bitset>
#include <GLFW/glfw3.h>

namespace glpp::system {

std::set<joystick_t> global_joysticks_registry;

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

std::strong_ordering joystick_t::operator<=>(const joystick_t& other) const {
	return m_id <=> other.m_id;
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

void glfw_resize_window_callback(GLFWwindow* window, int width, int height) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->m_height = height;
	win->m_width = width;
	win->input_handler().call_resize_action(width, height);
}

void glfw_cursor_position_callback(GLFWwindow* window, double x, double y) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	glm::vec2 mouse(
		(x/win->get_width())*2-1,
		(-y/win->get_height())*2+1
	);
	win->input_handler().call_mouse_move_action(mouse);
}

void glfw_enter_callback(GLFWwindow* window, int entered) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	if(entered) {
		win->input_handler().call_mouse_enter_action(win->get_mouse_pos());
	} else {
		win->input_handler().call_mouse_leave_action(win->get_mouse_pos());
	}
}

constexpr key_t glfw_to_key(int glfw_key) {
	switch(glfw_key) {
		case GLFW_KEY_UNKNOWN:
			return key_t::unknown;
		case GLFW_KEY_SPACE:
			return key_t::space;
		case GLFW_KEY_APOSTROPHE: /* ' */
			return key_t::apostrophe;
		case GLFW_KEY_COMMA: /* , */
			return key_t::comma;
		case GLFW_KEY_MINUS: /* - */
			return key_t::minus;
		case GLFW_KEY_PERIOD: /* . */
			return key_t::period;
		case GLFW_KEY_SLASH: /* / */
			return key_t::slash;
		case GLFW_KEY_0:
			return key_t::key_0;
		case GLFW_KEY_1:
			return key_t::key_1;
		case GLFW_KEY_2:
			return key_t::key_2;
		case GLFW_KEY_3:
			return key_t::key_3;
		case GLFW_KEY_4:
			return key_t::key_4;
		case GLFW_KEY_5:
			return key_t::key_5;
		case GLFW_KEY_6:
			return key_t::key_6;
		case GLFW_KEY_7:
			return key_t::key_7;
		case GLFW_KEY_8:
			return key_t::key_8;
		case GLFW_KEY_9:
			return key_t::key_9;
		case GLFW_KEY_SEMICOLON:
			return key_t::semicolon;
		case GLFW_KEY_EQUAL:
			return key_t::equal;
		case GLFW_KEY_A:
			return key_t::a;
		case GLFW_KEY_B:
			return key_t::b;
		case GLFW_KEY_C:
			return key_t::c;
		case GLFW_KEY_D:
			return key_t::d;
		case GLFW_KEY_E:
			return key_t::e;
		case GLFW_KEY_F:
			return key_t::f;
		case GLFW_KEY_G:
			return key_t::g;
		case GLFW_KEY_H:
			return key_t::h;
		case GLFW_KEY_I:
			return key_t::i;
		case GLFW_KEY_J:
			return key_t::j;
		case GLFW_KEY_K:
			return key_t::k;
		case GLFW_KEY_L:
			return key_t::l;
		case GLFW_KEY_M:
			return key_t::m;
		case GLFW_KEY_N:
			return key_t::n;
		case GLFW_KEY_O:
			return key_t::o;
		case GLFW_KEY_P:
			return key_t::p;
		case GLFW_KEY_Q:
			return key_t::q;
		case GLFW_KEY_R:
			return key_t::r;
		case GLFW_KEY_S:
			return key_t::s;
		case GLFW_KEY_T:
			return key_t::t;
		case GLFW_KEY_U:
			return key_t::u;
		case GLFW_KEY_V:
			return key_t::v;
		case GLFW_KEY_W:
			return key_t::w;
		case GLFW_KEY_X:
			return key_t::x;
		case GLFW_KEY_Y:
			return key_t::y;
		case GLFW_KEY_Z:
			return key_t::z;
		case GLFW_KEY_LEFT_BRACKET:
			return key_t::left_bracket;
		case GLFW_KEY_BACKSLASH:
			return key_t::backslash;
		case GLFW_KEY_RIGHT_BRACKET:
			return key_t::right_bracket;
		case GLFW_KEY_GRAVE_ACCENT:
			return key_t::grave_accent;
		case GLFW_KEY_WORLD_1: // non-us #1
			return key_t::world_1;
		case GLFW_KEY_WORLD_2: // non-us #2
			return key_t::world_2;
		case GLFW_KEY_ESCAPE:
			return key_t::escape;
		case GLFW_KEY_ENTER:
			return key_t::enter;
		case GLFW_KEY_TAB:
			return key_t::tab;
		case GLFW_KEY_BACKSPACE:
			return key_t::backspace;
		case GLFW_KEY_INSERT:
			return key_t::insert;
		case GLFW_KEY_DELETE:
			return key_t::del;
		case GLFW_KEY_RIGHT:
			return key_t::right;
		case GLFW_KEY_LEFT:
			return key_t::left;
		case GLFW_KEY_DOWN:
			return key_t::down;
		case GLFW_KEY_UP:
			return key_t::up;
		case GLFW_KEY_PAGE_UP:
			return key_t::page_up;
		case GLFW_KEY_PAGE_DOWN:
			return key_t::page_down;
		case GLFW_KEY_HOME:
			return key_t::home;
		case GLFW_KEY_END:
			return key_t::end;
		case GLFW_KEY_CAPS_LOCK:
			return key_t::caps_lock;
		case GLFW_KEY_SCROLL_LOCK:
			return key_t::scroll_lock;
		case GLFW_KEY_NUM_LOCK:
			return key_t::num_lock;
		case GLFW_KEY_PRINT_SCREEN:
			return key_t::print_screen;
		case GLFW_KEY_PAUSE:
			return key_t::pause;
		case GLFW_KEY_F1:
			return key_t::f1;
		case GLFW_KEY_F2:
			return key_t::f2;
		case GLFW_KEY_F3:
			return key_t::f3;
		case GLFW_KEY_F4:
			return key_t::f4;
		case GLFW_KEY_F5:
			return key_t::f5;
		case GLFW_KEY_F6:
			return key_t::f6;
		case GLFW_KEY_F7:
			return key_t::f7;
		case GLFW_KEY_F8:
			return key_t::f8;
		case GLFW_KEY_F9:
			return key_t::f9;
		case GLFW_KEY_F10:
			return key_t::f10;
		case GLFW_KEY_F11:
			return key_t::f11;
		case GLFW_KEY_F12:
			return key_t::f12;
		case GLFW_KEY_F13:
			return key_t::f13;
		case GLFW_KEY_F14:
			return key_t::f14;
		case GLFW_KEY_F15:
			return key_t::f15;
		case GLFW_KEY_F16:
			return key_t::f16;
		case GLFW_KEY_F17:
			return key_t::f17;
		case GLFW_KEY_F18:
			return key_t::f18;
		case GLFW_KEY_F19:
			return key_t::f19;
		case GLFW_KEY_F20:
			return key_t::f20;
		case GLFW_KEY_F21:
			return key_t::f21;
		case GLFW_KEY_F22:
			return key_t::f22;
		case GLFW_KEY_F23:
			return key_t::f23;
		case GLFW_KEY_F24:
			return key_t::f24;
		case GLFW_KEY_F25:
			return key_t::f25;
		case GLFW_KEY_KP_0:
			return key_t::num_0;
		case GLFW_KEY_KP_1:
			return key_t::num_1;
		case GLFW_KEY_KP_2:
			return key_t::num_2;
		case GLFW_KEY_KP_3:
			return key_t::num_3;
		case GLFW_KEY_KP_4:
			return key_t::num_4;
		case GLFW_KEY_KP_5:
			return key_t::num_5;
		case GLFW_KEY_KP_6:
			return key_t::num_6;
		case GLFW_KEY_KP_7:
			return key_t::num_7;
		case GLFW_KEY_KP_8:
			return key_t::num_8;
		case GLFW_KEY_KP_9:
			return key_t::num_9;
		case GLFW_KEY_KP_DECIMAL:
			return key_t::num_decimal;
		case GLFW_KEY_KP_DIVIDE:
			return key_t::num_divide;
		case GLFW_KEY_KP_MULTIPLY:
			return key_t::num_multiply;
		case GLFW_KEY_KP_SUBTRACT:
			return key_t::num_subtract;
		case GLFW_KEY_KP_ADD:
			return key_t::num_add;
		case GLFW_KEY_KP_ENTER:
			return key_t::num_enter;
		case GLFW_KEY_KP_EQUAL:
			return key_t::num_equal;
		case GLFW_KEY_LEFT_SHIFT:
			return key_t::left_shift;
		case GLFW_KEY_LEFT_CONTROL:
			return key_t::left_control;
		case GLFW_KEY_LEFT_ALT:
			return key_t::left_alt;
		case GLFW_KEY_LEFT_SUPER:
			return key_t::left_super;
		case GLFW_KEY_RIGHT_SHIFT:
			return key_t::right_shift;
		case GLFW_KEY_RIGHT_CONTROL:
			return key_t::right_control;
		case GLFW_KEY_RIGHT_ALT:
			return key_t::right_alt;
		case GLFW_KEY_RIGHT_SUPER:
			return key_t::right_super;
		case GLFW_KEY_MENU:
			return key_t::menu;
	};
	throw std::runtime_error("Unknown key code");
}

action_t glfw_to_action(int glfw_action) {
	switch(glfw_action) {
		case GLFW_PRESS:
			return action_t::press;
		case GLFW_RELEASE:
			return action_t::release;
	};
	throw std::runtime_error("Unknown action code");
}

void glfw_key_callback(GLFWwindow* window, int glfw_key, int, int glfw_action, int mods) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->input_handler().call_keyboard_action(glfw_to_key(glfw_key), glfw_to_action(glfw_action), mods);
}

mouse_button_t glfw_to_mouse_button(int glfw_button) {
	switch(glfw_button) {
		case GLFW_MOUSE_BUTTON_LEFT:
			return mouse_button_t::left;
		case GLFW_MOUSE_BUTTON_RIGHT:
			return mouse_button_t::right;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			return mouse_button_t::middle;
		case GLFW_MOUSE_BUTTON_4:
			return mouse_button_t::m4;
		case GLFW_MOUSE_BUTTON_5:
			return mouse_button_t::m5;
		case GLFW_MOUSE_BUTTON_6:
			return mouse_button_t::m6;
		case GLFW_MOUSE_BUTTON_7:
			return mouse_button_t::m7;
		case GLFW_MOUSE_BUTTON_8:
			return mouse_button_t::m8;
	};
	throw std::runtime_error("Unknown mouse button code");
}

void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->input_handler().call_mouse_action(glfw_to_mouse_button(button), glfw_to_action(action), mods);
}

void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	window_t* win = reinterpret_cast<window_t*>(glfwGetWindowUserPointer(window));
	win->input_handler().call_mouse_scroll_action(glm::vec2(xoffset, yoffset));
}

void glfw_joystick_function(int id, int event) {
	if(event == GLFW_CONNECTED) {
		global_joysticks_registry.emplace(id);
	}
	if(event == GLFW_DISCONNECTED) {
		global_joysticks_registry.erase(joystick_t(id));
	}
}


input_handler_t::input_handler_t(window_t& window) {
	glfwSetFramebufferSizeCallback(window.m_window, glfw_resize_window_callback);
	glfwSetKeyCallback(window.m_window, glfw_key_callback);
	glfwSetCursorPosCallback(window.m_window, glfw_cursor_position_callback);
	glfwSetCursorEnterCallback(window.m_window, glfw_enter_callback);
	glfwSetMouseButtonCallback(window.m_window, glfw_mouse_button_callback);
	glfwSetScrollCallback(window.m_window, glfw_scroll_callback);

	for(auto i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; ++i) {
		if(glfwJoystickPresent(i)) {
			global_joysticks_registry.emplace(i);
		}
	}
	glfwSetJoystickCallback(glfw_joystick_function);
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

std::set<joystick_t> input_handler_t::joysticks() const {
	return global_joysticks_registry;
}

glm::vec2 input_handler_t::get_mouse_position() const {
	return m_mouse_pos;
}

void input_handler_t::call_keyboard_action(key_t key, action_t event, int key_mod) {
	auto key_action_it = m_keyboard_actions.find({key, event});
	if(key_action_it != m_keyboard_actions.end()) {
		if(key_action_it->second) {
			key_action_it->second.operator()(key_mod);
		}
	}
}
void input_handler_t::call_mouse_action(mouse_button_t key, action_t event, int key_mod) {
	auto mouse_action_it = m_mouse_button.find({key, event});
	if(mouse_action_it != m_mouse_button.end()) {
		if(mouse_action_it->second) {
			mouse_action_it->second.operator()(m_mouse_pos, key_mod);
		}
	}
}
void input_handler_t::call_mouse_move_action(glm::vec2 dst) {
	auto src = m_mouse_pos;
	m_mouse_pos = dst;
	if(m_mouse_move_action) {
		m_mouse_move_action(dst, src);
	}
}

void input_handler_t::call_mouse_enter_action(glm::vec2 pos) {
	if(m_mouse_enter_action) {
		m_mouse_enter_action(pos);
	}
}

void input_handler_t::call_mouse_leave_action(glm::vec2 pos) {
	if(m_mouse_leave_action) {
		m_mouse_leave_action(pos);
	}
}

void input_handler_t::call_resize_action(double width, double height) {
	if(m_window_resize_action) {
		m_window_resize_action(width, height);
	}
}

void input_handler_t::call_mouse_scroll_action(glm::vec2 offset) {
	if(m_mouse_scroll_action) {
		m_mouse_scroll_action(offset);
	}
}

} // End of namespace glpp::system
