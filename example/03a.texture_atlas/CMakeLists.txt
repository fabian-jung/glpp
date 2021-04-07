add_executable(
	03a.texture_atlas 03a.texture_atlas.cpp
)
target_link_libraries(03a.texture_atlas PRIVATE glpp::core glpp::system glpp::image)

set(03a.copy.files 
	vertex.glsl
	fragment.glsl
	smiley.png
	one.png
	two.png
	three.png
	four.png
)

add_custom_target(03a.texture_atlas.copy
	DEPENDS ${CMAKE_CURRENT_LIST_DIR}/vertex.glsl
			${CMAKE_CURRENT_LIST_DIR}/fragment.glsl
)

foreach(file ${03a.copy.files})
	add_custom_target(${file} DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${file})
	add_dependencies(03a.texture_atlas.copy ${file})
	add_custom_command(
		TARGET 03a.texture_atlas.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_LIST_DIR}/${file}
				${CMAKE_CURRENT_BINARY_DIR}/${file}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${file}
	)
endforeach()

add_dependencies(03a.texture_atlas 03a.texture_atlas.copy)
