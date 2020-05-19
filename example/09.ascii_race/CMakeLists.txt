add_executable(09.ascii_race 09.ascii_race.cpp)
target_link_libraries(09.ascii_race PRIVATE glpp::core glpp::system glpp::text)

file(GLOB_RECURSE TEXT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl ${CMAKE_CURRENT_LIST_DIR}/*.ttf)

add_custom_target(09.ascii_race.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)


foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 09.ascii_race.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(09.ascii_race 09.ascii_race.copy)
