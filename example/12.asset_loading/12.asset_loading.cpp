
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
// #include <assimp/Logger.hpp>

// #include <glpp/core.hpp>
#include <fstream>

#include <glpp/system.hpp>
#include <glpp/asset.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glpp/object/texture.hpp>

#include <iostream>

int main(int, char*[]) {
	glpp::asset::importer_t importer("room.fbx");


	auto materials = importer.materials(std::cout);

	const auto lights = importer.all_lights();
	std::for_each(lights.begin(), lights.end(), [](const auto& light) {
		std::visit(
			[](const auto& l){
				std::cout << l << std::endl;
			},
			light
		);
	});

	using namespace glpp::system;
	using namespace glpp::object;
	using namespace glpp::render;

	window_t window(2000, 1000, "12.asset_loading", vsync_t::on);
	window.input_handler().set_keyboard_action(
		glpp::system::key_t::escape,
		action_t::press,
		[&](int){
			window.close();
		}
	);

	glpp::object::texture_t texture_one(
		glpp::object::image_t<glm::vec3>(
			1, 1,
			glm::vec3(1, 1, 1)
		),
		glpp::object::image_format_t::prefered,
		glpp::object::clamp_mode_t::clamp_to_edge,
		glpp::object::filter_mode_t::linear
	);
	auto texture_unit_one = texture_one.bind_to_texture_slot();

	struct scene_uniform_description_t {
		glm::mat4 mvp;
	};

	renderer_t<scene_uniform_description_t> renderer {
		shader_t(shader_type_t::vertex, std::ifstream("vertex.glsl")),
		shader_t(shader_type_t::fragment, std::ifstream("fragment.glsl"))
	};
	renderer.set_uniform_name(&scene_uniform_description_t::mvp, "mvp");
	renderer.set_texture("tile_tex" , texture_unit_one);

	std::vector<glpp::asset::mesh_t::model_t> models;
	for(const auto& mesh : importer.meshes()) {
		models.emplace_back(mesh.model());
	}

	auto cameras = importer.cameras();

	using model_t = decltype(models)::value_type;
	using vertex_description_t = typename model_traits<model_t>::attribute_description_t;
	using view_t = glpp::render::view_t<model_t>;
	std::vector<view_t> views;
	std::transform(models.begin(), models.end(), std::back_inserter(views), [](const auto& model){
		return glpp::render::view_t {
			model,
			&vertex_description_t::position,
			&vertex_description_t::normal,
			&vertex_description_t::tex
		};
	});

	glClearColor(0.2,0.2,0.2,1.0);

	int cam_count = 0;
	window.input_handler().set_keyboard_action(glpp::system::key_t::space, glpp::system::action_t::release, [&](int){
 		cam_count = (cam_count+1)%cameras.size();

	});
	// 	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glm::mat4 m(1.0f);
	window.enter_main_loop([&]() {
		auto& camera = cameras[cam_count];
		camera.aspect_ratio = window.get_aspect_ratio();

		renderer.set_uniform(&scene_uniform_description_t::mvp, camera.mvp(m));

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::for_each(views.begin(), views.end(), [&](const auto& view){
			renderer.render(view);
		});
	});

	return 0;
}
