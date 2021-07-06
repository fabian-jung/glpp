#pragma once

#include <functional>
#include <type_traits>
#include "mouse_action.hpp"

namespace glpp::ui::element {

template <
	class Child,
	class Action = std::function<void()>,
	class Enter = std::function<void(Child&)>,
	class Exit = std::function<void(Child&)>,
	class Press = std::function<void(Child&)>,
	class Release = std::function<void(Child&)>
>
class button_t {
public:
	Action action;
	Enter enter;
	Exit exit;
	Press press;
	Release release;

	Child& child() {
		return m_child.child;
	};

	const Child& child() const {
		return m_child.child;
	};

private:
	bool armed = false;

	struct action_t {
		button_t& button;
		bool armed = false;

		void operator()(mouse_event_t::press_t, glm::vec2){
			armed = true;
			button.press(button.child());
		}

		void operator()(mouse_event_t::release_t, glm::vec2){
			if(armed) {
				button.action();
				button.release(button.child());
			}
		}

		void operator()(mouse_event_t::enter_t, glm::vec2){
			if(!armed) {
				button.enter(button.child());
			}
		}

		void operator()(mouse_event_t::exit_t, glm::vec2){
			if(!armed) {
				button.exit(button.child());
			}
		}

		void operator()(mouse_event_t::remote_release_t) {
			if(armed) {
				button.release(button.child());
				button.exit(button.child());
			}
		}
	};

	using child_t = mouse_action_t<Child, action_t>;
	child_t m_child;

public:
	button_t(const button_t& cpy) :
		action(cpy.action),
		enter(cpy.enter),
		exit(cpy.exit),
		press(cpy.press),
		release(cpy.release),
		m_child(cpy.child(), action_t{*this})
	{}

	button_t(
		Child c,
		Action action,
		Enter enter,
		Exit exit,
		Press press,
		Release release
	) :
		action(std::move(action)),
		enter(std::move(enter)),
		exit(std::move(exit)),
		press(std::move(press)),
		release(std::move(release)),
		m_child(std::move(c), action_t{*this})
	{}

	using base_t = decltype(m_child);
	auto& base() {
		return m_child;
	}

	void textures(ui_base_t::textures_t& textures) {
		m_child.textures(textures);
	}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	) {
		m_child.triangulate(model, bottom_left, top_right, texture);
	}

	template <class Eve, class... Arg>
	void act(Eve event, const Arg&... args) {
		m_child.act(event, args...);
	}

};

template <
	class Child,
	class Action,
	class Enter,
	class Exit,
	class Press,
	class Release
>
button_t(Child, Action, Enter, Exit, Press, Release) -> button_t<Child, Action, Enter, Exit, Press, Release>;

}
