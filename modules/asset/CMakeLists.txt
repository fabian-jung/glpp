add_library(asset STATIC)
add_library(glpp::asset ALIAS asset)

file(GLOB_RECURSE glpp-asset-files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(asset PRIVATE ${glpp-asset-files})

target_include_directories(asset PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)

find_package(assimp REQUIRED)
target_link_directories(asset PUBLIC ${ASSIMP_LIBRARY_DIRS})
target_include_directories(asset PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(asset PUBLIC ${ASSIMP_LIBRARIES} glpp::core)
