find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS EGL)

add_library(system STATIC)
add_library(glpp::system ALIAS system)
target_include_directories(system PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(system PUBLIC glfw glpp::core)
target_link_libraries(system PRIVATE OpenGL::EGL)
set(glpp-system-files 
    ${CMAKE_CURRENT_LIST_DIR}/src/input.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/windowless_context.cpp
)
target_sources(system PRIVATE ${glpp-system-files})
target_compile_features(system PUBLIC cxx_std_20)
