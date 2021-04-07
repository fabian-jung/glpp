find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

add_library(core STATIC)
add_library(glpp::core ALIAS core)
target_include_directories(core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(core PUBLIC GLEW::GLEW OpenGL::OpenGL)
set(glpp-files
    ${CMAKE_CURRENT_LIST_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/glpp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader_factory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/texture_atlas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/vertex_array.cpp
)
target_sources(core PRIVATE ${glpp-files})
set_target_properties(core PROPERTIES PUBLIC_HEADER "${glpp-header}")
target_compile_features(core PUBLIC cxx_std_17)