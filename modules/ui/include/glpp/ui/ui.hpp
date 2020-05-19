#pragma once

#include "glpp/core.hpp"
#include "glpp/system/input.hpp"

namespace glpp::ui {

struct mouse_event_t {
	static constexpr struct press_t {} press {};
	static constexpr struct remote_press_t {} remote_press {};
	static constexpr struct release_t {} release {};
	static constexpr struct remote_release_t {} remote_release {};
	static constexpr struct move_t {} move {};
	static constexpr struct enter_t {} enter {};
	static constexpr struct exit_t {} exit {};
	static constexpr struct file_drop_t {} file_drop {};

};

template <class T>
struct is_mouse_event {
	static constexpr bool value = false;
};

template<> struct is_mouse_event<mouse_event_t::press_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::remote_press_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::release_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::remote_release_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::move_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::enter_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::exit_t> { static constexpr bool value = true; };
template<> struct is_mouse_event<mouse_event_t::file_drop_t> { static constexpr bool value = true; };

template <class T>
constexpr static bool is_mouse_event_v = is_mouse_event<T>::value;

template <class Lhs, class Rhs, std::enable_if_t<is_mouse_event_v<Lhs>&&is_mouse_event_v<Rhs>, int>* enable = nullptr>
constexpr bool operator==(const Lhs&, const Rhs&) {
	return std::is_same_v<Lhs, Rhs>;
}

class ui_base_t {
public:
	struct vertex_description_t {
		glm::vec2 pos;
		glm::vec2 tex {};
		GLint tex_id { -1 };
		glm::vec4 tint { 1.0f };
	};

	using model_t = glpp::render::model_t<vertex_description_t>;

	class textures_t {
	public:
		void insert(GLint id) {
			auto pos = std::lower_bound(m_data.begin(), m_data.end(), id);
			if(pos == m_data.end() || *pos != id) {
				m_data.insert(pos, id);
			}
		}

		void clear() {
			m_data.clear();
		}

		auto size() const {
			return m_data.size();
		}

		auto offset(GLuint id) const {
			return std::distance(m_data.begin(), std::lower_bound(m_data.begin(), m_data.end(), id));
		}

		auto begin() const {
			return m_data.begin();
		}

		auto end() const {
			return m_data.end();
		}

	private:
		std::vector<GLint> m_data;
	};

	void render() {
		renderer.render(view);
	}

	void update() {
		view = glpp::render::view_t<vertex_description_t> {
			model,
			&vertex_description_t::pos,
			&vertex_description_t::tex,
			&vertex_description_t::tex_id,
			&vertex_description_t::tint
		};
		renderer = {
			glpp::object::shader_t(
				glpp::object::shader_type_t::vertex,
				vertex_shader_code()
			),
			glpp::object::shader_t(
				glpp::object::shader_type_t::fragment,
				fragment_shader_code()
			)
		};
		renderer.set_texture_array("textures", textures.begin(), textures.end());
	}

protected:
	ui_base_t() :
		view(
			model,
			&vertex_description_t::pos,
			&vertex_description_t::tex,
			&vertex_description_t::tex_id,
			&vertex_description_t::tint
		)
	{}

	model_t model;
	textures_t textures;

private:
	std::string vertex_shader_code() const;
	std::string fragment_shader_code() const ;

	glpp::render::view_t<vertex_description_t> view;
	glpp::render::renderer_t<> renderer;
};

template <class Widget>
struct ui_t : public ui_base_t {

	ui_t(const ui_t& cpy) = delete; // not implemented yet
	ui_t& operator=(const ui_t& cpy) = delete; // not implemented yet

	ui_t(ui_t&& mov) = delete; // not implemented yet
	ui_t& operator=(ui_t&& mov) = delete; // not implemented yet


	ui_t(Widget widget, glpp::system::input_handler_t& input_handler) :
		widget(std::move(widget)),
		input_handler(&input_handler)
	{
		register_mouse();
		update();
	}

	~ui_t()
	{
		unregister_mouse();
	}

	void update() {
		textures.clear();
		widget.textures(textures);
		model.clear();
		widget.triangulate(model, glm::vec2(-1), glm::vec2(1), textures);
		ui_base_t::update();
	}

	void register_mouse() {
		input_handler->set_mouse_action(system::mouse_button_t::left, system::action_t::press, [this](glm::vec2 mouse, int){
			widget.act(mouse_event_t::press, mouse);
		});

		input_handler->set_mouse_action(system::mouse_button_t::left, system::action_t::release, [this](glm::vec2 mouse, int){
			widget.act(mouse_event_t::release, mouse);
		});

		input_handler->set_mouse_move_action([this](glm::vec2 dst, glm::vec2 src) {
			widget.act(mouse_event_t::move, dst, src);
		});

		input_handler->set_mouse_enter_action([this](glm::vec2 mouse) {
			widget.act(mouse_event_t::enter, mouse);
		});

		input_handler->set_mouse_leave_action([this](glm::vec2 mouse){
			widget.act(mouse_event_t::exit, mouse);
		});
	}

	void unregister_mouse() {

		input_handler->set_mouse_action(system::mouse_button_t::left, system::action_t::press, [](glm::vec2, int){
		});

		input_handler->set_mouse_action(system::mouse_button_t::left, system::action_t::release, [](glm::vec2, int){
		});

		input_handler->set_mouse_move_action([](glm::vec2, glm::vec2){
		});

		input_handler->set_mouse_enter_action([](glm::vec2) {
		});

		input_handler->set_mouse_leave_action([](glm::vec2) {
		});
	}

	Widget widget;

private:
	system::input_handler_t* input_handler;
};

template <class Widget>
ui_t(Widget, glpp::system::input_handler_t) -> ui_t<Widget>;

}
