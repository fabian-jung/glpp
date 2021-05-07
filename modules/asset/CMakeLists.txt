add_library(asset STATIC)
add_library(glpp::asset ALIAS asset)

set(glpp-asset-files
    ${CMAKE_CURRENT_LIST_DIR}/src/scene.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/importer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/light.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mesh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mesh_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/scene_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/depth.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/normal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/flat.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/blinn_phong.cpp
)
target_sources(asset PRIVATE ${glpp-asset-files})

target_include_directories(asset PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)

IF(POLICY CMP0012)
    CMAKE_POLICY(SET CMP0012 NEW)
ENDIF()
find_package(assimp REQUIRED)
target_link_directories(asset PUBLIC ${ASSIMP_LIBRARY_DIRS})
target_include_directories(asset PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(asset PUBLIC ${ASSIMP_LIBRARIES} glpp::core)

find_file(
    blender_executable "blender"
)
set(blender_export_scripts_dir ${CMAKE_CURRENT_LIST_DIR}/share CACHE PATH "Location of export scripts.")
if(EXISTS ${blender_executable})
    add_custom_target(blender_converter ALL)
    message(STATUS "Blender found on your system, enable blender_convert function.")
    function(blender_convert blend_file format)
        if(NOT EXISTS ${blend_file})
            message(FATAL_ERROR "File ${blend_file} not found.")
        endif()
        set(export_script_name "export.${format}.py")
        message(STATUS "python script name: ${export_script_name}")

        message(STATUS "search ${CMAKE_CURRENT_LIST_DIR} for export script.")
        unset(export_script CACHE)
        message(STATUS "export_script: ${export_script}")
        find_file(
            export_script "${export_script_name}"
            PATH ${CMAKE_CURRENT_SOURCE_DIR}/share
            ${blender_export_scripts_dir}
        )
        message(STATUS "export_script: ${export_script}")
        if(NOT EXISTS  ${export_script})
            message(FATAL_ERROR "Export format ${format} not supported. Try one of these: obj, fbx, glb.")
            return()
        endif()

        get_filename_component(blend_base ${blend_file} NAME_WLE)
        get_filename_component(blend_dir ${blend_file} DIRECTORY)
        get_filename_component(blend_name ${blend_file} NAME)
        
        set(output_path "${CMAKE_CURRENT_BINARY_DIR}/${blend_base}.${format}")
        add_custom_command(
            OUTPUT ${output_path}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${blend_file} ${export_script}
            COMMAND ${blender_executable} ${blend_file} --background --python ${export_script} -- ${output_path}
            COMMENT "Generate FBX ${CMAKE_CURRENT_BINARY_DIR}/${blend_base}.${format}"
            MAIN_DEPENDENCY ${blend_file}
        )
        add_custom_target(${blend_base}.${format} DEPENDS ${output_path})
        add_dependencies(blender_converter ${blend_base}.${format})
    endfunction()
endif()