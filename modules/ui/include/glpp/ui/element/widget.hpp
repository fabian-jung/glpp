#include "glpp/ui/ui.hpp"
#include <memory>

namespace glpp::ui::element {

namespace detail {

struct widget_impl_base_t {
	virtual ~widget_impl_base_t() = default;

	virtual widget_impl_base_t* clone() = 0;

	virtual void textures(ui_base_t::textures_t&) = 0;
	virtual void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	) = 0;

	virtual void act(mouse_event_t::press_t, glm::vec2 mouse) = 0;
	virtual void act(mouse_event_t::release_t, glm::vec2 mouse) = 0;
	virtual void act(mouse_event_t::move_t, glm::vec2 dst, glm::vec2 src) = 0;
	virtual void act(mouse_event_t::enter_t, glm::vec2 mouse) = 0;
	virtual void act(mouse_event_t::exit_t, glm::vec2 mouse) = 0;
};

template <class T>
struct widget_impl_t : public widget_impl_base_t {

	widget_impl_t(T v) :
		child(v)
	{}

	widget_impl_t* clone() override {
		return new widget_impl_t(child);
	}

	void textures(ui_base_t::textures_t& textures) override {
		return  child.textures(textures);
	}

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& textures
	) override {
		return child.triangulate(model, bottom_left, top_right, textures);
	}

	void act(mouse_event_t::press_t, glm::vec2 mouse) override {
		child.act(mouse_event_t::press, mouse);
	}

	void act(mouse_event_t::release_t, glm::vec2 mouse) override {
		child.act(mouse_event_t::release, mouse);
	}

	void act(mouse_event_t::move_t, glm::vec2 dst, glm::vec2 src) override {
		child.act(mouse_event_t::move, dst, src);
	}

	void act(mouse_event_t::enter_t, glm::vec2 mouse) override {
		child.act(mouse_event_t::enter, mouse);
	}

	void act(mouse_event_t::exit_t, glm::vec2 mouse) override {
		child.act(mouse_event_t::exit, mouse);
	}

	T child;
};

}

struct widget_t {

	widget_t() = default;

	template <class T>
	widget_t(T value) :
		child(std::make_unique<detail::widget_impl_t<T>>(value))
	{}

	widget_t(const widget_t& cpy);
	widget_t(widget_t&& mov) = default;

	widget_t& operator=(const widget_t& cpy);
	widget_t& operator=(widget_t&& mov) = default;


	void textures(ui_base_t::textures_t& textures);

	void triangulate(
		ui_base_t::model_t& model,
		const glm::vec2& bottom_left,
		const glm::vec2& top_right,
		const ui_base_t::textures_t& texture
	);

	template <class Eve, class... Args>
	void act(Eve event, const Args&... args) {
		if(child) child->act(event, args...);
	}

	std::unique_ptr<detail::widget_impl_base_t> child;

};

}
