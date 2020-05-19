#pragma once

#include "glpp/ui/ui.hpp"
#include <type_traits>
#include <map>

namespace glpp::ui::element {


namespace detail {

	template <typename... Ts>
	struct overload : Ts... { using Ts::operator()...; };

	template <typename... Ts>
	overload(Ts...) -> overload<Ts...>;

	template <class Action, class... Args>
	struct runtime_check_t {
		template <class T>
		constexpr static bool check(const T&) {
			return true;
		}
	};

	template <class... Args>
	struct runtime_check_t<std::function<void>(Args...), Args...> {

		static bool check(const std::function<void(Args...)>& fn) {
			return operator bool(fn);
		}
	};

	template <class... Actions, class... Args>
	struct runtime_check_t<overload<std::function<void(Args...)>, Actions...>, Args...> {

		template <class Overloaded>
		static bool check(const Overloaded& fn) {
			return static_cast<bool>( static_cast<const std::function<void(Args...)>&>(fn) );
		}
	};

	template <class... Actions, class Action, class... Args>
	struct runtime_check_t<overload<Action, Actions...>, Args...> {

		template <class Overloaded>
		static bool check(const Overloaded& fn) {
			return static_cast<bool>( static_cast<const std::function<void(Args...)>&>(fn) );
		}

	};

}

namespace detail {
	template<class... Ts> struct overloaded : Ts... {
		using Ts::operator()...;
	};
	template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

	template <class Functor, class ArgsTuple, class Enable = void>
	struct has_overload {
		constexpr static bool value = false;
	};

	template <class Functor, class... Args>
	struct has_overload<Functor, std::tuple<Args...>, std::invoke_result_t<Functor, Args...>> {
		constexpr static bool value = true;
	};

	template <class Functor, class... Args>
	constexpr bool has_overload_v = has_overload<Functor, std::tuple<Args...>>::value;
}

using generic_mouse_action_t = detail::overloaded<
	std::function<void(mouse_event_t::press_t, glm::vec2)>,
	std::function<void(mouse_event_t::remote_press_t, glm::vec2)>,
	std::function<void(mouse_event_t::release_t, glm::vec2)>,
	std::function<void(mouse_event_t::remote_release_t, glm::vec2)>,
	std::function<void(mouse_event_t::move_t, glm::vec2, glm::vec2 src)>,
	std::function<void(mouse_event_t::enter_t, glm::vec2)>,
	std::function<void(mouse_event_t::exit_t, glm::vec2)>
>;

template<class T, class Action>
struct mouse_action_t {

	T child;
	Action action;

	mouse_action_t(T child, Action action) :
		child(child),
		action(action)
	{}

	template <class... Actions>
	mouse_action_t(T child, Actions... actions) :
		child(child),
		action(detail::overloaded{actions...})
	{}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2 bottom_left,
		const glm::vec2 top_right,
		const ui_base_t::textures_t& textures
	) {
		if constexpr(!std::is_same_v<T, void>) {
			return  child.triangulate(model, bottom_left, top_right, textures);
		}
	}

	void textures(ui_base_t::textures_t& textures) {
		if constexpr(!std::is_same_v<T, void>) {
			child.textures(textures);
		}
	}

	template <class Event, class... Args>
	void act(Event event, const Args&... args) {
		if constexpr(detail::has_overload_v<Action, Event, Args...>) {
			if(detail::runtime_check_t<Action, Event, Args...>::check(action)) {
				action(event, args...);
				return;
			}
		}
		child.act(event, args...);
	}

};

template <class T, class... Actions>
mouse_action_t(T, Actions...) -> mouse_action_t<T, detail::overloaded<Actions...>>;

}
