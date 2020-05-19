#pragma once

#include <glpp/ui/ui.hpp>
#include <tuple>
#include <vector>

namespace glpp::ui::element {

template <class...  Ts>
struct multiplex_t {
	std::tuple<Ts...> elements;

	multiplex_t(Ts... elements) :
		elements {elements...}
	{}

	void textures(ui_base_t::textures_t& textures) {
		std::apply(
			[&](auto&... element) {
				( element.textures(textures), ... );
			},
			elements
		);
	}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& textures
	) {
		std::apply(
			[&](auto&... element) {
				([&](auto& e){
					e.triangulate(model, bottom_left, top_right, textures);
				}(element), ...);
			},
			elements
		);
	}

	template <class Eve, class... Args>
	void act(Eve event, const Args&... args) {
		std::apply(
			[&](auto&... element) {
				(element.act(event, args...), ...);
			},
			elements
		);
	}

};

template <class... Ts>
multiplex_t(Ts...) -> multiplex_t<Ts...>;

template <class  T>
struct multiplex_list_t {
	std::vector<T> elements;

	template <class... Args>
	multiplex_list_t(const Args&&... args) :
		elements(std::forward<Args>(args)...)
	{
	}

	void textures(ui_base_t::textures_t& textures) {
		for(const auto& e : elements) {
			const auto part = e.textures();
			for(const auto v : part) {
				textures.insert(v);
			}
		}
	}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& textures
	) {
		for(const auto& e : elements) {
			e.triangulate(model, bottom_left, top_right, textures);
		}
	}

	template <class Eve, class... Args>
	void act(Eve event, const Args&... args) {
		for(const auto& e : elements) {
			e.act(event, args...);
		}
	}
};

template <class... Args>
multiplex_list_t(Args...) -> multiplex_list_t<typename decltype(std::vector {std::declval<Args>()...})::value_type>;

}

