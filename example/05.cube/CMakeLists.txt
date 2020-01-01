add_executable(05.cube 05.cube.cpp)
target_link_libraries(05.cube PRIVATE glpp::core glpp::system)

file(GLOB FRAMEBUFFER_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl)

add_custom_target(05.cube.copy
	DEPENDS ${FRAMEBUFFER_DEPENDENCIES}
)


foreach(res ${FRAMEBUFFER_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 05.cube.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_LIST_DIR}/${name}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(05.cube 05.cube.copy)
