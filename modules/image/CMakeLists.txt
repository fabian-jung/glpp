add_subdirectory(vendor)

add_library(image STATIC)
add_library(glpp::image ALIAS image)
target_include_directories(image PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(image PUBLIC glpp::core)
target_link_libraries(image PRIVATE stb)
set(glpp-image-files
	${CMAKE_CURRENT_LIST_DIR}/src/texture.cpp
)
target_sources(image PRIVATE ${glpp-image-files})
target_compile_features(image PUBLIC cxx_std_20)
