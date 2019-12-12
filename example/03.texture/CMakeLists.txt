add_executable(
	03.texture 03.texture.cpp
)
target_link_libraries(03.texture PRIVATE glpp::core glpp::system)

add_custom_target(03.texture.copy
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
			${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
)

add_custom_command(
	TARGET 03.texture.copy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
			${CMAKE_CURRENT_BINARY_DIR}/vertex.glsl
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
)

add_custom_command(
	TARGET 03.texture.copy POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
			${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
			${CMAKE_CURRENT_BINARY_DIR}/fragment.glsl
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
)

add_dependencies(03.texture 03.texture.copy)
