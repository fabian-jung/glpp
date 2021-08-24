option(enable_asset "Build with the asset module, for loading 3d assets from files." ON)
option(enable_system "Build with the system module, for creating windows." ON)
option(enable_image "Build with the image module, for loading images into textures." ON)
option(enable_text "Build with the system module, for loading fonts and rendering text." ON)
option(enable_ui "Build with the ui module, for displaying widgets like buttons, frames and labels." ON)

if(${enable_unit_test})
add_subdirectory(testing)
endif()

add_subdirectory(gl)

add_subdirectory(core)

if(${enable_asset})
add_subdirectory(asset)
endif()

if(${enable_system})
add_subdirectory(system)
endif()

if(${enable_image})
add_subdirectory(image)
endif()

if(${enable_text})
add_subdirectory(text)
endif()

if(${enable_ui})
add_subdirectory(ui)
endif()
