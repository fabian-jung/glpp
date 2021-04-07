add_executable(08.text_box 08.text_box.cpp)
target_link_libraries(08.text_box PRIVATE glpp::core glpp::system glpp::text)

set(TEXT_DEPENDENCIES
	${CMAKE_CURRENT_LIST_DIR}/ttf/Hack-Regular.ttf
)

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
