from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps

class ArkConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("fmt/12.0.0", override=True)
        self.requires("spdlog/1.15.3")
        self.requires("tracy/0.13.1")

    def layout(self):
        pass
