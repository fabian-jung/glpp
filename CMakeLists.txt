cmake_minimum_required(VERSION 3.16)

project(mapnify)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})

option(enable_examples "Build with example applications" ON)



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
