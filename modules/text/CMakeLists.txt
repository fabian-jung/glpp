find_package(Freetype REQUIRED)

add_library(text STATIC)
add_library(glpp::text ALIAS text)
target_include_directories(text PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(text PUBLIC glpp::core Freetype::Freetype)
file(GLOB_RECURSE glpp-text-files ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
target_sources(text PRIVATE ${glpp-text-files})
target_compile_features(text PUBLIC cxx_std_17)

