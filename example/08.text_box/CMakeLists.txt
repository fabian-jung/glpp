add_executable(08.text_box 08.text_box.cpp)
target_link_libraries(08.text_box PRIVATE glpp::core glpp::system glpp::text)
target_compile_features(08.text_box PUBLIC cxx_std_20)

file(GLOB_RECURSE TEXT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl ${CMAKE_CURRENT_LIST_DIR}/*.ttf)

add_custom_target(08.text_box.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)


foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 08.text_box.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(08.text_box 08.text_box.copy)
