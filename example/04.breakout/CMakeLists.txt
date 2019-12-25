add_executable(
	04.breakout 04.breakout.cpp ball_motion_handler.cpp
)
target_link_libraries(04.breakout PRIVATE glpp::core glpp::system glpp::image)

file(GLOB BREAKOUT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/assets/*.glsl ${CMAKE_CURRENT_LIST_DIR}/assets/*.png ${CMAKE_CURRENT_LIST_DIR}/assets/*.bmp)

add_custom_target(04.breakout.copy
	DEPENDS ${BREAKOUT_DEPENDENCIES}
)


foreach(res ${BREAKOUT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 04.breakout.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_LIST_DIR}/assets/${name}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/assets/${name}
	)
endforeach()

add_dependencies(04.breakout 04.breakout.copy)
