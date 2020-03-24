option(enable_system "Build with the system module, for creating windows." ON)
option(enable_image "Build with the image module, for loading images into textures." ON)
option(enable_text "Build with the system module, for loading fonts and rendering text." ON)

if(${enable_system})
add_subdirectory(system)
endif()

if(${enable_image})
add_subdirectory(image)
endif()

if(${enable_text})
add_subdirectory(text)
endif()
