set(Test_TESTS
    ${CMAKE_CURRENT_LIST_DIR}/context.cpp
)

if(${enable_unit_test})
foreach(file ${Test_TESTS})
	get_filename_component(name ${file} NAME_WLE)
    add_executable(${name}_test ${file})
    target_link_libraries(${name}_test PRIVATE glpp::testing)
    catch_discover_tests(${name}_test)
endforeach()
endif()