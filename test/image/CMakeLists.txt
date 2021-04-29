set(IMAGE_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/image_io.cpp
)

if(${enable_unit_test})
foreach(file ${IMAGE_TESTS})
	get_filename_component(name ${file} NAME_WLE)
    add_executable(${name}_test ${file})
    target_link_libraries(${name}_test PRIVATE glpp::testing glpp::core glpp::image)
    catch_discover_tests(${name}_test)
endforeach()
endif()