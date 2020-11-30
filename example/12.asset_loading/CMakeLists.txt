add_executable(12.asset_loading 12.asset_loading.cpp)
target_link_libraries(12.asset_loading PRIVATE glpp::core glpp::system glpp::asset glpp::image)


file(GLOB_RECURSE ASSET_LOADING_ASSETS ${CMAKE_CURRENT_LIST_DIR}/*.blend)

set(blender_executable /usr/bin/blender CACHE FILEPATH "PATH of the blender executable used to compile scenes" FORCE)
set(FBX_FILES "")

foreach(res ${ASSET_LOADING_ASSETS})
	get_filename_component(name ${res} NAME_WLE)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx
		DEPENDS ${name}.blend
		COMMAND ${blender_executable} ${res} --background --python ${CMAKE_CURRENT_LIST_DIR}/export_fbx.py -- ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx
		COMMENT "Generate FBX ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx"
		MAIN_DEPENDENCY ${res}
	)
	list(APPEND FBX_FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx")
endforeach()

add_custom_target(12.asset_loading.fbx_target DEPENDS ${FBX_FILES})
add_dependencies(12.asset_loading 12.asset_loading.fbx_target)


file(GLOB ASSET_LOADING_DEPENDENCIES ${CMAKE_CURRENT_LIST_DIR}/*.glsl)

add_custom_target(12.asset_loading.copy
	DEPENDS ${ASSET_LOADING_DEPENDENCIES}
)


foreach(res ${ASSET_LOADING_DEPENDENCIES})
	get_filename_component(name ${res} NAME)
	add_custom_command(
		TARGET 12.asset_loading.copy POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy
				${CMAKE_CURRENT_LIST_DIR}/${name}
				${CMAKE_CURRENT_BINARY_DIR}/${name}
		DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${name}
	)
endforeach()

add_dependencies(12.asset_loading 12.asset_loading.copy)
