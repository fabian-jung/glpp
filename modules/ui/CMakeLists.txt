add_library(ui STATIC)
add_library(glpp::ui ALIAS ui)
target_include_directories(ui PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:/usr/local/include>
)
target_link_libraries(ui PUBLIC glpp::core glpp::system glpp::text)
set(glpp-ui-files
    ${CMAKE_CURRENT_LIST_DIR}/src/frame.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/image.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ui.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/widget.cpp
)
target_sources(ui PRIVATE ${glpp-ui-files})
target_compile_features(ui PUBLIC cxx_std_20)