#pragma once

#include <map>
#include <functional>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

namespace glpp::system {


enum class key_t : int;

enum class mouse_button_t : int;

enum class action_t : int {
	press = GLFW_PRESS,
	release = GLFW_RELEASE
};

class window_t;

class joystick_t {
public:

	joystick_t(int id);
	int id() const;
	const char* name() const;
	bool is_gamepad() const;
	int axes_count() const;
	int button_count() const;

	float axes(int axis) const;
	unsigned char buttons(int btn) const;

private:
	int m_id;
	int m_axis_count;
	int m_button_count;
};

class input_handler_t {
public:

	using key_action_callback_t = std::function<void(int key_mod)>;
	using mouse_action_callback_t = std::function<void(glm::vec2, int key_mod)>;
	using mouse_move_action_t = std::function<void(glm::vec2 dst, glm::vec2 src)>;
	using mouse_enter_action_t = std::function<void(glm::vec2)>;
	using mouse_leave_action_t = std::function<void(glm::vec2)>;
	using window_resize_action_t = std::function<void(double width, double height)>;
	using mouse_scroll_action_t = std::function<void(glm::vec2)>;
	input_handler_t(window_t& window);

	void set_keyboard_action(key_t key, action_t event, key_action_callback_t callback);
	void set_mouse_action(mouse_button_t key, action_t event, mouse_action_callback_t callback);
	void set_mouse_move_action(mouse_move_action_t callback);
	void set_mouse_enter_action(mouse_enter_action_t callback);
	void set_mouse_leave_action(mouse_leave_action_t callback);
	void set_resize_action(window_resize_action_t callback);
	void set_mouse_scroll_action(mouse_scroll_action_t callback);
	const std::vector<joystick_t>& joysticks() const;

	friend window_t;

private:
	using key_action_t = std::tuple<key_t, action_t>;
	using mouse_action_t = std::tuple<mouse_button_t, action_t>;

	static void glfw_error_callback(int error, const char* description);
	static void glfw_resize_window_callback(GLFWwindow* window, int width, int height);
	static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
	static void glfw_enter_callback(GLFWwindow* window, int entered);
	static void glfw_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void glfw_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void glfw_joystick_function(int id, int event);

	glm::vec2 m_mouse_pos {};

	std::map<key_action_t, key_action_callback_t> m_keyboard_actions;
	std::map<mouse_action_t, mouse_action_callback_t> m_mouse_button;
	mouse_move_action_t m_mouse_move_action;
	mouse_enter_action_t m_mouse_enter_action;
	mouse_leave_action_t m_mouse_leave_action;
	window_resize_action_t m_window_resize_action;
	mouse_scroll_action_t m_mouse_scroll_action;
	static std::vector<joystick_t> m_joysticks;
};

enum class key_t : int {
	unknown				=   GLFW_KEY_UNKNOWN,
	space				=   GLFW_KEY_SPACE,
	apostrophe			=   GLFW_KEY_APOSTROPHE, /* ' */
	comma				=   GLFW_KEY_COMMA, /* , */
	minus				=   GLFW_KEY_MINUS, /* - */
	period				=   GLFW_KEY_PERIOD, /* . */
	slash				=   GLFW_KEY_SLASH, /* / */
	key_0				=   GLFW_KEY_0,
	key_1				=   GLFW_KEY_1,
	key_2				=   GLFW_KEY_2,
	key_3				=   GLFW_KEY_3,
	key_4				=   GLFW_KEY_4,
	key_5				=   GLFW_KEY_5,
	key_6				=   GLFW_KEY_6,
	key_7				=   GLFW_KEY_7,
	key_8				=   GLFW_KEY_8,
	key_9				=   GLFW_KEY_9,
	semicolon			=   GLFW_KEY_SEMICOLON,
	equal				=   GLFW_KEY_EQUAL,
	a					=   GLFW_KEY_A,
	b					=   GLFW_KEY_B,
	c					=   GLFW_KEY_C,
	d					=   GLFW_KEY_D,
	e					=   GLFW_KEY_E,
	f					=   GLFW_KEY_F,
	g					=   GLFW_KEY_G,
	h					=   GLFW_KEY_H,
	i					=   GLFW_KEY_I,
	j					=   GLFW_KEY_J,
	k					=   GLFW_KEY_K,
	l					=   GLFW_KEY_L,
	m					=   GLFW_KEY_M,
	n					=   GLFW_KEY_N,
	o					=   GLFW_KEY_O,
	p					=   GLFW_KEY_P,
	q					=   GLFW_KEY_Q,
	r					=   GLFW_KEY_R,
	s					=   GLFW_KEY_S,
	t					=   GLFW_KEY_T,
	u					=   GLFW_KEY_U,
	v					=   GLFW_KEY_V,
	w					=   GLFW_KEY_W,
	x					=   GLFW_KEY_X,
	y					=   GLFW_KEY_Y,
	z					=   GLFW_KEY_Z,
	left_bracket		=   GLFW_KEY_LEFT_BRACKET,
	backslash			=   GLFW_KEY_BACKSLASH,
	right_bracke		=   GLFW_KEY_RIGHT_BRACKET,
	grave_accent		=   GLFW_KEY_GRAVE_ACCENT,
	world_1				=   GLFW_KEY_WORLD_1, // non-us #1
	world_2				=   GLFW_KEY_WORLD_2, // non-us #2
	escape				=   GLFW_KEY_ESCAPE,
	enter				=   GLFW_KEY_ENTER,
	tab					=   GLFW_KEY_TAB,
	backspace			=   GLFW_KEY_BACKSPACE,
	insert				=   GLFW_KEY_INSERT,
	del					=   GLFW_KEY_DELETE,
	right				=   GLFW_KEY_RIGHT,
	left				=   GLFW_KEY_LEFT,
	down				=   GLFW_KEY_DOWN,
	up					=   GLFW_KEY_UP,
	page_up				=   GLFW_KEY_PAGE_UP,
	page_down			=   GLFW_KEY_PAGE_DOWN,
	home				=   GLFW_KEY_HOME,
	end					=   GLFW_KEY_END,
	caps_lock			=   GLFW_KEY_CAPS_LOCK,
	scroll_lock			=   GLFW_KEY_SCROLL_LOCK,
	num_lock			=   GLFW_KEY_NUM_LOCK,
	print_screen		=   GLFW_KEY_PRINT_SCREEN,
	pause				=   GLFW_KEY_PAUSE,
	f1					=   GLFW_KEY_F1,
	f2					=   GLFW_KEY_F2,
	f3					=   GLFW_KEY_F3,
	f4					=   GLFW_KEY_F4,
	f5					=   GLFW_KEY_F5,
	f6					=   GLFW_KEY_F6,
	f7					=   GLFW_KEY_F7,
	f8					=   GLFW_KEY_F8,
	f9					=   GLFW_KEY_F9,
	f10					=   GLFW_KEY_F10,
	f11					=   GLFW_KEY_F11,
	f12					=   GLFW_KEY_F12,
	f13					=   GLFW_KEY_F13,
	f14					=   GLFW_KEY_F14,
	f15					=   GLFW_KEY_F15,
	f16					=   GLFW_KEY_F16,
	f17					=   GLFW_KEY_F17,
	f18					=   GLFW_KEY_F18,
	f19					=   GLFW_KEY_F19,
	f20					=   GLFW_KEY_F20,
	f21					=   GLFW_KEY_F21,
	f22					=   GLFW_KEY_F22,
	f23					=   GLFW_KEY_F23,
	f24					=   GLFW_KEY_F24,
	f25					=   GLFW_KEY_F25,
	num_0				=   GLFW_KEY_KP_0,
	num_1				=   GLFW_KEY_KP_1,
	num_2				=   GLFW_KEY_KP_2,
	num_3				=   GLFW_KEY_KP_3,
	num_4				=   GLFW_KEY_KP_4,
	num_5				=   GLFW_KEY_KP_5,
	num_6				=   GLFW_KEY_KP_6,
	num_7				=   GLFW_KEY_KP_7,
	num_8				=   GLFW_KEY_KP_8,
	num_9				=   GLFW_KEY_KP_9,
	num_decimal			=   GLFW_KEY_KP_DECIMAL,
	num_divide			=   GLFW_KEY_KP_DIVIDE,
	num_multiply		=   GLFW_KEY_KP_MULTIPLY,
	num_subtract		=   GLFW_KEY_KP_SUBTRACT,
	num_add				=   GLFW_KEY_KP_ADD,
	num_enter			=   GLFW_KEY_KP_ENTER,
	num_equal			=   GLFW_KEY_KP_EQUAL,
	left_shift			=   GLFW_KEY_LEFT_SHIFT,
	left_control		=   GLFW_KEY_LEFT_CONTROL,
	left_alt			=   GLFW_KEY_LEFT_ALT,
	left_super			=   GLFW_KEY_LEFT_SUPER,
	right_shift			=   GLFW_KEY_RIGHT_SHIFT,
	right_contro		=   GLFW_KEY_RIGHT_CONTROL,
	right_alt			=   GLFW_KEY_RIGHT_ALT,
	right_super			=   GLFW_KEY_RIGHT_SUPER,
	menu				=   GLFW_KEY_MENU
};

enum class mouse_button_t : int {
	left   = GLFW_MOUSE_BUTTON_LEFT,
	right  = GLFW_MOUSE_BUTTON_RIGHT,
	middle = GLFW_MOUSE_BUTTON_MIDDLE,
	m4     = GLFW_MOUSE_BUTTON_4,
	m5     = GLFW_MOUSE_BUTTON_5,
	m6     = GLFW_MOUSE_BUTTON_6,
	m7     = GLFW_MOUSE_BUTTON_7,
	m8     = GLFW_MOUSE_BUTTON_8
};

} // end of namespace glpp::system
