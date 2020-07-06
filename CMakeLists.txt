cmake_minimum_required(VERSION 3.16)

project(mapnify)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})

option(enable_examples "Build with example applications" ON)

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

add_subdirectory(modules)

if(${enable_examples})
add_subdirectory(example)
endif()

include(GNUInstallDirs)
install(TARGETS core system stb image EXPORT glppConfig LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
install(EXPORT glppConfig FILE glppTargets.cmake NAMESPACE glpp:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glpp EXPORT_LINK_INTERFACE_LIBRARIES)
install(FILES glppConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glpp)
install(DIRECTORY include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/system/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
