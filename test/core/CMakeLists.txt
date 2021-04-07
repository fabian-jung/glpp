set(CORE_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/object.cpp
    ${CMAKE_CURRENT_LIST_DIR}/model.cpp
)

if(${enable_unit_test})
foreach(file ${CORE_TESTS})
	get_filename_component(name ${file} NAME_WLE)
    add_executable(${name}_test ${file})
    target_link_libraries(${name}_test PRIVATE glpp::testing glpp::core)
    catch_discover_tests(${name}_test)
endforeach()
endif()