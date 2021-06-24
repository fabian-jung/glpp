include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
ExternalProject_Add(
	Boost_pfr-src
	GIT_REPOSITORY "https://github.com/boostorg/pfr.git"

	GIT_TAG "master"
	UPDATE_COMMAND ""
	PATCH_COMMAND ""
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""

	SOURCE_DIR "${CMAKE_BINARY_DIR}/vendor/boost_pfr"
)

add_library(Boost_pfr INTERFACE)
add_library(Boost::pfr ALIAS Boost_pfr)
target_compile_features(Boost_pfr INTERFACE cxx_std_14)
target_include_directories(Boost_pfr INTERFACE ${CMAKE_BINARY_DIR}/vendor/boost_pfr/include)