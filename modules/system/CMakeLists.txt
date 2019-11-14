add_library(system STATIC)
add_library(glpp::system ALIAS system)
target_include_directories(system PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_include_directories(system PRIVATE ${GLFW_INCLUDE_DIRS})
target_link_libraries(system PUBLIC  ${GLFW_LIBRARIES} GLEW::GLEW OpenGL::OpenGL glpp::core)
file(GLOB_RECURSE glpp-system-files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(system PRIVATE ${glpp-system-files})
target_compile_features(system PUBLIC cxx_std_17)
