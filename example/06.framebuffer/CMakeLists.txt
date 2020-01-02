add_executable(06.framebuffer 06.framebuffer.cpp)
target_link_libraries(06.framebuffer PRIVATE glpp::core glpp::system)

file(GLOB FRAMEBUFFER_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl)

add_custom_target(06.framebuffer.copy
	DEPENDS ${FRAMEBUFFER_DEPENDENCIES}
)


foreach(res ${FRAMEBUFFER_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 06.framebuffer.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_LIST_DIR}/${name}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(06.framebuffer 06.framebuffer.copy)
