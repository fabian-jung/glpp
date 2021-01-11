add_executable(12.asset_loading 12.asset_loading.cpp)
target_link_libraries(12.asset_loading PRIVATE glpp::core glpp::system glpp::asset glpp::image)


file(GLOB_RECURSE ASSET_LOADING_ASSETS ${CMAKE_CURRENT_LIST_DIR}/*.blend)

set(blender_executable /usr/bin/blender CACHE FILEPATH "PATH of the blender executable used to compile scenes" FORCE)
set(FBX_FILES "")

foreach(res ${ASSET_LOADING_ASSETS})
	get_filename_component(name ${res} NAME_WLE)
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx
		DEPENDS ${name}.blend export_fbx.py
		COMMAND ${blender_executable} ${res} --background --python ${CMAKE_CURRENT_LIST_DIR}/export_fbx.py -- ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx
		COMMENT "Generate FBX ${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx"
		MAIN_DEPENDENCY ${res}
	)
	list(APPEND FBX_FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}.fbx")
	add_custom_command(
		OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${name}.glb
		DEPENDS ${name}.blend export_gltf.py
		COMMAND ${blender_executable} ${res} --background --python ${CMAKE_CURRENT_LIST_DIR}/export_gltf.py -- ${CMAKE_CURRENT_BINARY_DIR}/${name}.glb
		COMMENT "Generate GLB ${CMAKE_CURRENT_BINARY_DIR}/${name}.glb"
		MAIN_DEPENDENCY ${res}
	)
	list(APPEND FBX_FILES "${CMAKE_CURRENT_BINARY_DIR}/${name}.glb")
endforeach()

add_custom_target(12.asset_loading.fbx_target DEPENDS ${FBX_FILES})
add_dependencies(12.asset_loading 12.asset_loading.fbx_target)
