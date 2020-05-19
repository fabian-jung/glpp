add_executable(07.text 07.text.cpp)
target_link_libraries(07.text PRIVATE glpp::core glpp::system glpp::text)

file(GLOB_RECURSE TEXT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl ${CMAKE_CURRENT_LIST_DIR}/*.ttf)

add_custom_target(07.text.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)


foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 07.text.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(07.text 07.text.copy)

