
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
// #include <assimp/Logger.hpp>

// #include <glpp/core.hpp>
#include <fstream>

#include <glpp/system.hpp>
#include <glpp/asset.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glpp/core.hpp>

#include <iostream>
#include <memory>

int main(int, char*[]) {

	using namespace glpp::system;
	using namespace glpp::core::object;
	using namespace glpp::asset;
	using namespace glpp::asset::render;
	using namespace glpp::asset::shading;

	window_t window(2000, 1000, "12.asset_loading", vsync_t::on);
	window.input_handler().set_keyboard_action(
		glpp::system::key_t::escape,
		action_t::press,
		[&](int){
			window.close();
		}
	);

	scene_t scene(
		"room.fbx"
	);
	auto& cameras = scene.cameras;

	multi_atlas_t texture_atlas;
	scene_view_t sview(scene);

	// scene_renderer_t srenderer { flat_t { texture_atlas, flat_shading_channel_t::diffuse }, scene };
	scene_renderer_t srenderer { normal_t{}, scene };

	glClearColor(0.2,0.2,0.2,1.0);
	auto cam_it = cameras.begin();
	window.input_handler().set_keyboard_action(glpp::system::key_t::space, glpp::system::action_t::release, [&](int){
 		++cam_it;
		if(cam_it == cameras.end()) {
			cam_it = cameras.begin();
		}
	});

	// window.input_handler().set_keyboard_action(glpp::system::key_t::tab, glpp::system::action_t::release, [&, toggle = true](int) mutable {
	// 	if(toggle) {
	// 		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// 	} else {
	// 		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	// 	}
	// 	toggle = !toggle;
	// });

	// glEnable(GL_DEPTH_TEST);

	window.enter_main_loop([&]() {
		cam_it->aspect_ratio = window.get_aspect_ratio();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		srenderer.render(sview, *cam_it);
	});

	return 0;
}
