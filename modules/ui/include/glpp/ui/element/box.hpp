#pragma once

#include "glpp/ui/ui.hpp"

namespace glpp::ui::element {

template <class T>
struct box_t {

	glm::vec2 bottom_left = glm::vec2(-1), top_right = glm::vec2(1);
	T child;

	constexpr box_t(T child) :
		child(child)
	{}

	constexpr box_t(glm::vec2 size, T child) :
		bottom_left(-0.5f*size),
		top_right(0.5f*size),
		child(child)
	{}

	constexpr box_t(glm::vec2 bottom_left, glm::vec2 top_right, T child) :
		bottom_left(bottom_left),
		top_right(top_right),
		child(child)
	{}

	void textures(ui_base_t::textures_t& textures) {
		child.textures(textures);
	}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	) {
		const auto center = 0.5f*(bottom_left+top_right);
		const auto size = top_right-bottom_left;
		const auto scale = 0.5f*size;
		const auto bl = this->bottom_left*scale+center;
		const auto tr = this->top_right*scale+center;
		child.triangulate(model, bl, tr, texture);
	}

	void act(mouse_event_t::move_t, const glm::vec2& dst, const glm::vec2& src) {
		const auto dst_mouse = inside_transform(dst);
		const auto src_mouse = inside_transform(src);

		const bool dst_inside = is_inside(dst_mouse);
		const bool src_inside = is_inside(src_mouse);
		if(dst_inside) {
			if(src_inside) {
				child.act(mouse_event_t::move, dst_mouse, src_mouse);
			} else {
				child.act(mouse_event_t::enter, dst_mouse);
			}
		} else {
			if(src_inside) {
				child.act(mouse_event_t::exit, dst_mouse);
			}
		}
	}

	template <class Eve, class... Args>
	void act(Eve event, const glm::vec2 pos, const Args&... args) {
		const auto inner_mouse = inside_transform(pos);
		if(is_inside(inner_mouse)) {
			child.act(event, inner_mouse, args...);
		} else {
			if constexpr(event == mouse_event_t::press) {
				child.act(mouse_event_t::remote_press);
			}
			if constexpr(event == mouse_event_t::release) {
				child.act(mouse_event_t::remote_release);
			}
		}
	}

	template <class Eve>
	void act(Eve event) {
		// Remote release/press events
		child.act(event);
	}

private:
	glm::vec2 inside_transform(glm::vec2 outside) {
		const glm::vec2 center = 0.5f*(bottom_left+top_right);
		const glm::vec2 size = top_right-bottom_left;
		return (outside - center)/(0.5f*size);
	}

	bool is_inside(glm::vec2 inside_position) {
		return glm::all(glm::lessThanEqual(glm::abs(inside_position), glm::vec2(1.0f)));
	}
};

template <class T>
box_t(glm::vec2, glm::vec2, T) -> box_t<T>;

template <class T>
box_t(glm::vec2, T) -> box_t<T>;

template <class T>
box_t(T) -> box_t<T>;

}
