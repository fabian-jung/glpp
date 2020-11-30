add_library(asset STATIC)
add_library(glpp::asset ALIAS asset)

set(glpp-asset-files
    ${CMAKE_CURRENT_LIST_DIR}/src/scene.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/light.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mesh.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/mesh_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/scene_view.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/depth.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/normal.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/flat.cpp
    # ${CMAKE_CURRENT_LIST_DIR}/src/blinn_phong.cpp
)
target_sources(asset PRIVATE ${glpp-asset-files})

target_include_directories(asset PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)

IF(POLICY CMP0012)
    CMAKE_POLICY(SET CMP0012 NEW)
ENDIF()
find_package(assimp REQUIRED)
target_link_directories(asset PUBLIC ${ASSIMP_LIBRARY_DIRS})
target_include_directories(asset PUBLIC ${ASSIMP_INCLUDE_DIRS})
target_link_libraries(asset PUBLIC ${ASSIMP_LIBRARIES} glpp::core)