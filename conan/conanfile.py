import os

from conan import tools, ConanFile
from conan.tools.files import get, copy

class IpToolsConan(ConanFile):
    name = "iptools"
    description = "Header only library of IP utilities. Uses features of C++11."
    topics = ("conan", "net")
    url = "https://github.com/hoxnox/iptools"
    homepage =  "https://github.com/hoxnox/iptools"
    license = "https://github.com/hoxnox/iptools/blob/master/LICENSE"
    package_type="header-library"
    settings = "compiler"
    version = "0.5.0"

    def source(self):
        get(self, **self.conan_data["sources"][self.version], strip_root=True)

    def package(self):
        copy(self, "*.hpp", 
             src=os.path.join(self.source_folder, "include", "iptools"),
             dst=os.path.join(self.package_folder, "include", "iptools"))

    def package_infp(self):
        self.cpp_info.includedirs = ["include"]

