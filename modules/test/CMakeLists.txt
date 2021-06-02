add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)

find_package(OpenGL REQUIRED COMPONENTS EGL)

add_library(glpp_test)
target_link_libraries(glpp_test PUBLIC catch_main glpp::gl glpp::core glpp::system OpenGL::EGL)
add_library(glpp::testing ALIAS glpp_test)

set(glpp-glpp_test-files
    ${CMAKE_CURRENT_LIST_DIR}/src/context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/offscreen_driver.cpp
)
target_sources(glpp_test PRIVATE ${glpp-glpp_test-files})
target_include_directories(glpp_test PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)