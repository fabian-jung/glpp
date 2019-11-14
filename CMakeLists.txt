cmake_minimum_required(VERSION 3.11)

project(mapnify)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})


find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(GLEW REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

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

add_subdirectory(modules)
add_subdirectory(example)

include(GNUInstallDirs)
install(TARGETS core system EXPORT glppConfig LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install(EXPORT glppConfig FILE glppTargets.cmake NAMESPACE glpp:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glpp EXPORT_LINK_INTERFACE_LIBRARIES)
install(FILES glppConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glpp)
install(DIRECTORY include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/system/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
# export(PACKAGE OpenGL)
# export(TARGETS core system NAMESPACE glpp:: FILE glppConfig.cmake EXPORT_LINK_INTERFACE_LIBRARIES)
# configure_file(glppTargets.cmake.in glppConfig.cmake @ONLY)
# install(
# 	FILES "${CMAKE_CURRENT_BINARY_DIR}/glppConfig.cmake"
# 	DESTINATION lib/cmake/glpp
# )
