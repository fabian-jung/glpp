add_library(core STATIC)
add_library(glpp::core ALIAS core)
target_include_directories(core PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(core PUBLIC fmt::fmt glpp::gl Boost::headers glm::glm)

set(glpp-files
    ${CMAKE_CURRENT_LIST_DIR}/src/camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/framebuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/glpp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/shader_factory.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/texture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/texture_atlas.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/vertex_array.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/grid.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/multi.cpp
)
target_sources(core PRIVATE ${glpp-files})
set_target_properties(core PROPERTIES PUBLIC_HEADER "${glpp-header}")
target_compile_features(core PUBLIC cxx_std_20)
target_compile_definitions(core PUBLIC -DGLM_ENABLE_EXPERIMENTAL)