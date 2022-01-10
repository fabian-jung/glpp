include(CMakeFindDependencyMacro)

# Capturing values from configure (optional)
set(my-config-var @my-config-var@)

# Same syntax as find_package
find_dependency(OpenGL REQUIRED COMPONENTS OpenGL)
find_dependency(Boost REQUIRED)
find_dependency(Catch2 REQUIRED)
find_dependency(fmt REQUIRED)
find_dependency(assimp REQUIRED)

include(CTest NO_POLICY_SCOPE)
include(Catch NO_POLICY_SCOPE)
include(ParseAndAddCatchTests)

find_dependency(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

# Any extra setup

# Add the targets file
include(${CMAKE_CURRENT_LIST_DIR}/BlenderExport.cmake)
include("${CMAKE_CURRENT_LIST_DIR}/glppTargets.cmake")
