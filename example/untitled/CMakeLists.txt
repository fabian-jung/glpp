add_executable(untitled untitled.cpp)
target_link_libraries(untitled PRIVATE glpp::core glpp::system glpp::ui)
target_compile_features(untitled PUBLIC cxx_std_20)

file(GLOB_RECURSE TEXT_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl ${CMAKE_CURRENT_LIST_DIR}/*.ttf)

add_custom_target(untitled.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)

foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET untitled.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(untitled untitled.copy)
