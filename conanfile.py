import os

from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import copy

class GlppRecipe(ConanFile):
    # Binary configuration
    name = "glpp"
    version = "0.1.0"
    package_type = "static-library"

    license = "MIT"
    author = "Fabian Jung <git@fabian-jung.net>"
    url = "https://github.com/fabian-jung/glpp"
    description = "OpenGL C++ Wrapper"
    topics = ("OpenGL", "wrapper", "c++", "rendering", "graphics")

    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": True, "fPIC": True}

    exports_sources = "CMakeLists.txt", "CPackLists.txt", "example/*", "glppConfig.cmake", "BlenderExport.cmake", "modules/*", "test/*"

    def requirements(self):
        self.requires("opengl/system", transitive_headers=True)
        self.requires("egl/system", transitive_headers=True)
        self.requires("fmt/11.0.2", transitive_headers=True)
        self.requires("assimp/5.3.1", transitive_headers=True)
        self.requires("glm/cci.20230113", transitive_headers=True)
        self.requires("freetype/2.13.2", transitive_headers=True)
        self.requires("glfw/3.4", transitive_headers=True)
        self.requires("glew/2.2.0", transitive_headers=True)
        self.requires("boost/1.85.0", transitive_headers=True)
        # self.requires("stb/cci.20230920")
        self.requires("catch2/3.6.0", transitive_headers=True)

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
        # copy(self, "BlenderExport.cmake", src="{self.source_folder}/modules/asset/", dst=self.package_folder)

    def package_info(self):
        # self.cpp_info.builddirs = ["."]

        self.cpp_info.libs = ["gl", "core", "system", "stb", "image", "testing", "text", "ui", "asset"]
        self.cpp_info.names["cmake_find_package"] = "glpp"

        self.cpp_info.components["core"].libs = ["core"]
        self.cpp_info.components["core"].libdirs = ["lib/glpp"]
        self.cpp_info.components["core"].set_property("cmake_target_name", "glpp::core")
        self.cpp_info.components["core"].requires = ["fmt::fmt", "gl", "boost::headers", "glm::glm"]

        self.cpp_info.components["gl"].libs = ["gl"]
        self.cpp_info.components["gl"].libdirs = ["lib/glpp"]
        self.cpp_info.components["gl"].set_property("cmake_target_name", "glpp::gl")
        self.cpp_info.components["gl"].requires = ["glew::glew", "opengl::opengl"]

        self.cpp_info.components["system"].libs = ["system"]
        self.cpp_info.components["system"].libdirs = ["lib/glpp"]
        self.cpp_info.components["system"].set_property("cmake_target_name", "glpp::system")
        self.cpp_info.components["system"].requires = ["glfw::glfw", "egl::egl"] # "glfw"

        self.cpp_info.components["stb"].libs = ["stb"]
        self.cpp_info.components["stb"].libdirs = ["lib/glpp"]
        self.cpp_info.components["stb"].set_property("cmake_target_name", "stb")

        self.cpp_info.components["image"].libs = ["image"]
        self.cpp_info.components["image"].libdirs = ["lib/glpp"]
        self.cpp_info.components["image"].set_property("cmake_target_name", "glpp::image")
        self.cpp_info.components["image"].requires = ["core", "stb"]

        self.cpp_info.components["testing"].libs = ["testing"]
        self.cpp_info.components["testing"].libdirs = ["lib/glpp"]
        self.cpp_info.components["testing"].set_property("cmake_target_name", "glpp::testing")
        self.cpp_info.components["testing"].requires = ["core", "catch2::catch2_with_main", "gl", "system", "image"]

        self.cpp_info.components["text"].libs = ["text"]
        self.cpp_info.components["text"].libdirs = ["lib/glpp"]
        self.cpp_info.components["text"].set_property("cmake_target_name", "glpp::text")
        self.cpp_info.components["text"].requires = ["core", "image", "freetype::freetype"]

        self.cpp_info.components["ui"].libs = ["ui"]
        self.cpp_info.components["ui"].libdirs = ["lib/glpp"]
        self.cpp_info.components["ui"].set_property("cmake_target_name", "glpp::ui")
        self.cpp_info.components["ui"].requires = ["core", "system", "image", "text"]

        self.cpp_info.components["asset"].libs = ["asset"]
        self.cpp_info.components["asset"].libdirs = ["lib/glpp"]
        self.cpp_info.components["asset"].set_property("cmake_target_name", "glpp::asset")
        self.cpp_info.components["asset"].requires = ["assimp::assimp", "core", "image", "text", "ui"]
