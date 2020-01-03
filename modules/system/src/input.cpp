#include "glpp/system/input.hpp"

namespace glpp::system {

void input_handler_t::set_keyboard_action(key_t key, action_t event, key_action_callback_t callback) {
	m_keyboard_actions[{key, event}] = std::move(callback);
}

void input_handler_t::set_mouse_action(mouse_button_t key, action_t event, mouse_action_callback_t callback) {
	m_mouse_button[{key, event}] = std::move(callback);
}

void input_handler_t::set_mouse_move_action(mouse_move_action_t callback) {
	m_mouse_move_action = std::move(callback);
}

void input_handler_t::set_resize_action(glpp::system::input_handler_t::window_resize_action_t callback) {
	m_window_resize_action = std::move(callback);
}

} // End of namespace glpp::system
