include(CMakeFindDependencyMacro)

# Capturing values from configure (optional)
set(my-config-var @my-config-var@)

# Same syntax as find_package
find_dependency(OpenGL REQUIRED COMPONENTS OpenGL)
find_dependency(GLEW REQUIRED)

find_dependency(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# Any extra setup

# Add the targets file
include("${CMAKE_CURRENT_LIST_DIR}/glppTargets.cmake")
