import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps

class GlppRecipe(ConanFile):
    # Binary configuration
    name = "glpp"
    version = "0.1"

    license = "MIT"
    author = "Fabian Jung <git@fabian-jung.net>"
    url = "https://github.com/fabian-jung/glpp"
    description = "OpenGL C++ Wrapper"
    topics = ("OpenGL", "wrapper", "c++", "rendering", "graphics")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}

    exports_sources = "CMakeLists.txt", "CPackLists.txt", "example/*", "glppConfig.cmake", "modules/*", "test/*"

    def requirements(self):
        self.requires("opengl/system")
        self.requires("egl/system")
        self.requires("fmt/10.2.1")
        self.requires("assimp/5.3.1")
        self.requires("glm/cci.20230113")
        self.requires("freetype/2.13.2")
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")
        self.requires("boost/1.85.0")
        self.requires("stb/cci.20230920")
        self.requires("catch2/3.6.0")

    def build_requirements(self):
        self.tool_requires("cmake/3.29.3")

    def layout(self):
        self.folders.source = "."
        self.folders.generators = "conan_generators"
        self.folders.build = "build"
        self.folders.install = "install"

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["glpp"]