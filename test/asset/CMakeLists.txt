set(ASSET_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/import.cpp
    ${CMAKE_CURRENT_LIST_DIR}/render.cpp
)

if(${enable_unit_test})
foreach(file ${ASSET_TESTS})
	get_filename_component(name ${file} NAME_WLE)
    add_executable(${name}_test ${file})
    target_link_libraries(${name}_test PRIVATE glpp::testing glpp::core glpp::asset)
    catch_discover_tests(${name}_test)
endforeach()

blender_convert(${CMAKE_CURRENT_LIST_DIR}/cube.blend fbx)
blender_convert(${CMAKE_CURRENT_LIST_DIR}/cube.blend glb)
endif()