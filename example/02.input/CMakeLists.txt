add_executable(
	02.input 02.input.cpp
)
target_link_libraries(02.input PRIVATE glpp::core glpp::system)

add_custom_target(02.input.copy
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
			${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
)

add_custom_command(
	TARGET 02.input.copy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
			${CMAKE_CURRENT_BINARY_DIR}/vertex.glsl
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
)

add_custom_command(
	TARGET 02.input.copy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
			${CMAKE_CURRENT_BINARY_DIR}/fragment.glsl
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
)

add_dependencies(02.input 02.input.copy)
