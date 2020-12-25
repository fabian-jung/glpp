find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

add_library(core STATIC)
add_library(glpp::core ALIAS core)
target_include_directories(core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(core PUBLIC GLEW::GLEW OpenGL::OpenGL)
file(GLOB_RECURSE glpp-files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(core PRIVATE ${glpp-files})
set_target_properties(core PROPERTIES PUBLIC_HEADER "${glpp-header}")
target_compile_features(core PUBLIC cxx_std_17)

if(${enable_unit_test})
add_executable(camera_test test/camera_test)
target_link_libraries(camera_test PRIVATE glpp::testing glpp::core)
catch_discover_tests(camera_test)
endif()
