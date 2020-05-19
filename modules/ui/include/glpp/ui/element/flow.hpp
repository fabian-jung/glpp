#pragma once

#include <tuple>
#include <iterator>
#include <vector>
#include <array>
#include <utility>

#include "multiplex.hpp"
#include "box.hpp"

namespace glpp::ui::element {

enum class flow_direction_t {
	vertical,
	horizontal
};

template <class... Elements>
struct flow_t {
private:
	constexpr static size_t size = sizeof...(Elements);

	constexpr static auto bottom_left() {
		return glm::vec2{-1.0f};
	}

	constexpr static auto top_right(flow_direction_t direction) {
		if(direction == flow_direction_t::vertical) {
			return glm::vec2{1.0, -1.0}+offset(direction);
		} else {
			return glm::vec2{-1.0, 1.0}+offset(direction);
		}
	}

	constexpr static auto offset(flow_direction_t direction) {
		if(direction == flow_direction_t::vertical) {
			return glm::vec2{0, 2.0f/static_cast<float>(size)};
		} else {
			return glm::vec2{2.0f/static_cast<float>(size), 0};
		}
	}

	template< size_t... I>
	constexpr flow_t(
		std::index_sequence<I...>,
		flow_direction_t direction,
		Elements... elements
	) :
		elements {
			box_t {
				bottom_left()+offset(direction)*static_cast<float>(I),
				top_right(direction)+offset(direction)*static_cast<float>(I),
				elements
			}...
		}
	{}

public:
	multiplex_t<box_t<Elements>...> elements;

	template <size_t N>
	auto& get() {
		return std::get<N>(elements.elements).child;
	}

	template <class seq_t = std::make_index_sequence<sizeof...(Elements)>>
	constexpr flow_t(
		flow_direction_t direction,
		Elements... elements
	) :
		flow_t {
			seq_t{},
			direction,
			elements...
		}
	{}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	) {
		elements.triangulate(model, bottom_left, top_right, texture);
	}

	void textures(ui_base_t::textures_t& textures) {
		elements.textures(textures);
	}

	template <class Eve, class... Args>
	void act(Eve event, const Args&... args) {
		elements.act(event, args...);
	}

};

template <class... Elements>
flow_t(flow_direction_t, std::tuple<Elements...>) -> flow_t<Elements...>;

template <class... Elements>
flow_t(flow_direction_t,  Elements&&...) -> flow_t<Elements...>;

template <size_t count>
struct static_flow_policy_t {
	template <class T>
	using type = std::array<T, count>;

	static constexpr size_t size = count;

};

struct dynamic_flow_policy_t {
	template <class T>
	using type = std::vector<T>;
};

template <class Element, class SizePolicy = dynamic_flow_policy_t>
struct flow_list_t {
	template <class T>
	using container_t = typename SizePolicy::template type<T>;

	flow_direction_t direction;
	container_t<Element> elements;

	constexpr explicit flow_list_t(
		flow_direction_t direction
	) :
		direction(direction)
	{}

	constexpr flow_list_t(
		flow_direction_t direction,
		std::initializer_list<Element> elements
	) :
		direction(direction),
		elements(elements)
	{}

	constexpr flow_list_t(
		SizePolicy,
		flow_direction_t direction,
		std::initializer_list<Element> elements
	) :
		direction(direction),
		elements(elements)
	{}

	template <size_t N,
		std::enable_if_t<
			std::is_same_v<SizePolicy, static_flow_policy_t<N>>,
			int
		> _ = 0
	>
	constexpr flow_list_t(
		static_flow_policy_t<N>,
		flow_direction_t direction,
		Element init
	) :
		flow_list_t(direction,  std::make_index_sequence<N>{}, init)
	{}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	) {
		const glm::vec2 size = top_right-bottom_left;
		const float number_of_elements = elements.size();
		const glm::vec2 offset =
			(direction == flow_direction_t::vertical) ?
				glm::vec2{0, size.y/number_of_elements} :
				glm::vec2{size.x/number_of_elements, 0};

		glm::vec2 bl = bottom_left;
		glm::vec2 tr =
			(direction == flow_direction_t::vertical) ?
				glm::vec2{top_right.x, bl.y+offset.y} :
				glm::vec2{bl.x+offset.x, top_right.y};

		for(auto& e : elements) {
			e.triangulate(model, bl, tr, texture);
			bl += offset;
			tr += offset;
		}
	}

	void textures(ui_base_t::textures_t& textures) {
		for(auto& e : elements) {
			e.textures(textures);
		}
	}

	template <class Eve, class... Args>
	void act(Eve event, const Args&... args) {
		const float number_of_elements = elements.size();
		const glm::vec2 offset =
			(direction == flow_direction_t::vertical) ?
				glm::vec2{0, 2.0f/number_of_elements} :
				glm::vec2{2.0f/number_of_elements, 0};

		glm::vec2 bl {-1.0f};
		glm::vec2 tr =
			(direction == flow_direction_t::vertical) ?
				glm::vec2{1.0f, bl.y+offset.y} :
				glm::vec2{bl.x+offset.x, 1.0f};

		for(auto& e : elements) {
			box_t<Element&> wrapper_box { bl, tr, e };
			wrapper_box.act(event, args...);
			bl += offset;
			tr += offset;
		}
	}

private:
	template<size_t... I>
	constexpr flow_list_t(
		flow_direction_t direction,
		std::index_sequence<I...>,
		Element init
	) :
		direction(direction),
		elements {{ [&](size_t){ return init; }(I) ... }}
	{}
};

template <class Element>
flow_list_t(flow_direction_t, std::initializer_list<Element>) -> flow_list_t<Element>;

template <class Element, class SizePolicy>
flow_list_t(SizePolicy, flow_direction_t, std::initializer_list<Element>) -> flow_list_t<Element, SizePolicy>;

template <size_t N, class Element>
flow_list_t(static_flow_policy_t<N>, flow_direction_t, Element) -> flow_list_t<Element, static_flow_policy_t<N>>;

}
