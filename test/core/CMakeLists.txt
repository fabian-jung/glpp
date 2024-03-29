set(CORE_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/object.cpp
    ${CMAKE_CURRENT_LIST_DIR}/model.cpp
    ${CMAKE_CURRENT_LIST_DIR}/attribute_properties.cpp
    ${CMAKE_CURRENT_LIST_DIR}/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vertex_array.cpp
    ${CMAKE_CURRENT_LIST_DIR}/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader_program.cpp
    ${CMAKE_CURRENT_LIST_DIR}/shader_factory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texture_atlas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texture_atlas_render.cpp
    ${CMAKE_CURRENT_LIST_DIR}/view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/renderer.cpp
)

if(${enable_unit_test})
foreach(file ${CORE_TESTS})
	get_filename_component(name ${file} NAME_WLE)
    add_executable(${name}_test ${file})
    target_link_libraries(${name}_test PRIVATE glpp::testing glpp::core)
    catch_discover_tests(${name}_test)
endforeach()
endif()