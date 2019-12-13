include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(
	stb-src
	GIT_REPOSITORY "https://github.com/nothings/stb.git"

	GIT_TAG "master"
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""

	SOURCE_DIR "${CMAKE_BINARY_DIR}/vendor/stb"
)

add_library(stb STATIC vendor/stb.cpp)
target_include_directories(stb PUBLIC ${CMAKE_BINARY_DIR}/vendor/stb)
target_compile_features(stb PUBLIC cxx_std_17)


add_library(image STATIC)
add_library(glpp::image ALIAS image)
target_include_directories(image PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(image PUBLIC GLEW::GLEW OpenGL::OpenGL glpp::core)
target_link_libraries(image PRIVATE stb)
file(GLOB_RECURSE glpp-image-files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(image PRIVATE ${glpp-image-files})
target_compile_features(image PUBLIC cxx_std_17)

