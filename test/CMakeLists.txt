if(${enable_unit_test})
    add_subdirectory(test)
    add_subdirectory(core)

    if(${enable_image})
    add_subdirectory(image)
    endif()

    if(${enable_asset})
    add_subdirectory(asset)
    endif()

endif()