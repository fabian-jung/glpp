add_executable(glpp_context_generator)
target_sources(glpp_context_generator PUBLIC 
    ${CMAKE_CURRENT_LIST_DIR}/src/definitions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/constants.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/types.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/functions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bin/generator.cpp
)
target_include_directories(glpp_context_generator PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_compile_features(glpp_context_generator PUBLIC cxx_std_20)

set(GLPP_GL_ROOT "${CMAKE_BINARY_DIR}/vendor/glpp/modules/gl")
file(MAKE_DIRECTORY ${GLPP_GL_ROOT})
add_custom_command(
    OUTPUT
        ${GLPP_GL_ROOT}/include/glpp/gl/types.hpp
        ${GLPP_GL_ROOT}/include/glpp/gl/constants.hpp
        ${GLPP_GL_ROOT}/include/glpp/gl/functions.hpp
        ${GLPP_GL_ROOT}/include/glpp/gl/context.hpp
        ${GLPP_GL_ROOT}/include/glpp/gl.hpp
        ${GLPP_GL_ROOT}/src/context.cpp
        ${GLPP_GL_ROOT}/src/functions.cpp
        ${GLPP_GL_ROOT}/src/gl.cpp
    COMMAND
        ${CMAKE_CURRENT_BINARY_DIR}/glpp_context_generator
    DEPENDS
        glpp_context_generator
    WORKING_DIRECTORY
        ${GLPP_GL_ROOT}
)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

add_library(gl)
add_library(glpp::gl ALIAS gl)
target_compile_features(gl PUBLIC cxx_std_20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
target_compile_options(gl PRIVATE "-fno-var-tracking-assignments")
endif()

option(glew_enabled "Load dynamic opengl functions with glew (experimental feature, untested)" OFF)

target_include_directories(gl PUBLIC
    $<BUILD_INTERFACE:${GLPP_GL_ROOT}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_definitions(gl PUBLIC -DGLFW_INCLUDE_NONE)

if(${glew_enabled})
    find_package(GLEW REQUIRED)
    target_link_libraries(gl PUBLIC GLEW OpenGL::OpenGL)
    target_compile_definitions(gl PUBLIC -DWITH_GLEW)
elseif()  
    target_link_libraries(gl PRIVATE OpenGL::OpenGL)
endif()
target_sources(gl PRIVATE 
    ${GLPP_GL_ROOT}/src/context.cpp
    ${GLPP_GL_ROOT}/src/functions.cpp
    ${GLPP_GL_ROOT}/src/gl.cpp
)
