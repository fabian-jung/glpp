add_executable(10.ui 10.ui.cpp)
target_link_libraries(10.ui PRIVATE glpp::core glpp::image glpp::system glpp::ui)

set(TEXT_DEPENDENCIES
	${CMAKE_CURRENT_LIST_DIR}/ttf/Hack-Regular.ttf
	${CMAKE_CURRENT_LIST_DIR}/add.png
	${CMAKE_CURRENT_LIST_DIR}/remove.png
)

add_custom_target(10.ui.copy
	DEPENDS ${TEXT_DEPENDENCIES}
)

foreach(res ${TEXT_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 10.ui.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${res}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(10.ui 10.ui.copy)