add_executable(11.lights_out 11.lights_out.cpp)
target_link_libraries(11.lights_out PRIVATE glpp::core glpp::image glpp::system glpp::ui)

file(GLOB_RECURSE TEXT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.png ${CMAKE_CURRENT_LIST_DIR}/*.ttf)

add_custom_target(11.lights_out.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)

foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 11.lights_out.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(11.lights_out 11.lights_out.copy)
