#prama once

#include "scene.hpp"

namespace glpp::asset {

class scene_view_t {
public:
	explicit scene_view_t(const scene_t& scene);

private:

};

class scene_renderer_t {
public:
	scene_renderer_t();

	void render(const scene_view_t&);

private:

};

}
