cmake_minimum_required(VERSION 3.16)

project(glpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES /usr/local/lib ${CMAKE_CXX_IMPLICIT_LINK_DIRECTORIES})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

option(enable_unit_test "Build with unit tests applications" ON)
if(${enable_unit_test})
	find_package(Catch2 REQUIRED)
	include(CTest NO_POLICY_SCOPE)
	include(Catch NO_POLICY_SCOPE)
	include(ParseAndAddCatchTests)
endif()

add_subdirectory(modules)
if(${enable_unit_test})
	add_subdirectory(test)
endif()

option(enable_examples "Build with example applications" ON)
if(${enable_examples})
	add_subdirectory(example)
endif()

file(GLOB_RECURSE GLPP_FORMAT_SET *.hpp *.cpp)
add_custom_target(check-format
	COMMAND clang-format --dry-run -Werror ${GLPP_FORMAT_SET}
)

add_custom_target(format
	COMMAND clang-format -i ${GLPP_FORMAT_SET}
)

ADD_CUSTOM_TARGET(doc_tagfile_link
	COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_SOURCE_DIR}/glpp.tagfile.xml ${CMAKE_BINARY_DIR}/glpp.tagfile.xml
)

add_custom_target(doc
	COMMAND poxy ${CMAKE_SOURCE_DIR}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	DEPENDS doc_tagfile_link
)

install(TARGETS gl core system stb image testing text ui EXPORT glppConfig DESTINATION lib/glpp)
if(${enable_asset})
install(TARGETS asset EXPORT glppConfig DESTINATION lib/glpp)
endif()

install(EXPORT glppConfig NAMESPACE glpp:: FILE glppTargets.cmake NAMESPACE glpp:: DESTINATION lib/cmake/glpp EXPORT_LINK_INTERFACE_LIBRARIES)
install(FILES glppConfig.cmake DESTINATION lib/cmake/glpp)
install(FILES modules/asset/BlenderExport.cmake DESTINATION lib/cmake/glpp)
install(DIRECTORY ${CMAKE_BINARY_DIR}/vendor/glpp/modules/gl/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/core/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/image/include/glpp DESTINATION cmakincludee FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/system/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/testing/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/text/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/asset/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")
install(DIRECTORY modules/asset/share DESTINATION share/glpp FILES_MATCHING PATTERN "*")
install(DIRECTORY modules/ui/include/glpp DESTINATION include FILES_MATCHING PATTERN "*.h*")

include(CPackLists.txt)