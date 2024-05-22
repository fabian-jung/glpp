#pragma once

#include <map>
#include <set>
#include <functional>

#include <glm/glm.hpp>

namespace glpp::system {


enum class key_t : int;

enum class mouse_button_t : int;

enum class action_t : int {
	press,
	release,
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

	std::strong_ordering operator<=>(const joystick_t&) const;
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
	std::set<joystick_t> joysticks() const;

	glm::vec2 get_mouse_position() const;

	void call_keyboard_action(key_t key, action_t event, int key_mod);
	void call_mouse_action(mouse_button_t key, action_t event, int key_mod);
	void call_mouse_move_action(glm::vec2 dst);
	void call_mouse_enter_action(glm::vec2 pos);
	void call_mouse_leave_action(glm::vec2 pos);
	void call_resize_action(double width, double height);
	void call_mouse_scroll_action(glm::vec2 offset);

private:
	friend window_t;

	using key_action_t = std::tuple<key_t, action_t>;
	using mouse_action_t = std::tuple<mouse_button_t, action_t>;

	glm::vec2 m_mouse_pos {};

	std::map<key_action_t, key_action_callback_t> m_keyboard_actions;
	std::map<mouse_action_t, mouse_action_callback_t> m_mouse_button;
	mouse_move_action_t m_mouse_move_action;
	mouse_enter_action_t m_mouse_enter_action;
	mouse_leave_action_t m_mouse_leave_action;
	window_resize_action_t m_window_resize_action;
	mouse_scroll_action_t m_mouse_scroll_action;
};

enum class key_t : int {
	unknown,
	space,
	apostrophe, /* ' */
	comma, /* ,  */
	minus, /* - */
	period, /* . */
	slash, /* / */
	key_0,
	key_1,
	key_2,
	key_3,
	key_4,
	key_5,
	key_6,
	key_7,
	key_8,
	key_9,
	semicolon,
	equal,
	a,
	b,
	c,
	d,
	e,
	f,
	g,
	h,
	i,
	j,
	k,
	l,
	m,
	n,
	o,
	p,
	q,
	r,
	s,
	t,
	u,
	v,
	w,
	x,
	y,
	z,
	left_bracket,
	backslash,
	right_bracket,
	grave_accent,
	world_1, // non-us #1
	world_2, // non-us #2
	escape,
	enter,
	tab,
	backspace,
	insert,
	del,
	right,
	left,
	down,
	up,
	page_up,
	page_down,
	home,
	end,
	caps_lock,
	scroll_lock,
	num_lock,
	print_screen,
	pause,
	f1,
	f2,
	f3,
	f4,
	f5,
	f6,
	f7,
	f8,
	f9,
	f10,
	f11,
	f12,
	f13,
	f14,
	f15,
	f16,
	f17,
	f18,
	f19,
	f20,
	f21,
	f22,
	f23,
	f24,
	f25,
	num_0,
	num_1,
	num_2,
	num_3,
	num_4,
	num_5,
	num_6,
	num_7,
	num_8,
	num_9,
	num_decimal,
	num_divide,
	num_multiply,
	num_subtract,
	num_add,
	num_enter,
	num_equal,
	left_shift,
	left_control,
	left_alt,
	left_super,
	right_shift,
	right_control,
	right_alt,
	right_super,
	menu
};

enum class mouse_button_t : int {
	left,
	right,
	middle,
	m4,
	m5,
	m6,
	m7,
	m8
};

} // end of namespace glpp::system
