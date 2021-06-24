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

add_library(stb STATIC stb.cpp)
target_include_directories(stb PUBLIC
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/vendor/stb>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_compile_features(stb PUBLIC cxx_std_20)
add_dependencies(stb stb-src)