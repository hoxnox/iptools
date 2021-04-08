from conans import tools, ConanFile

class IpToolsConan(ConanFile):
    name = "iptools"
    description = "Header only library of IP utilities. Uses features of C++11."
    topics = ("conan", "net")
    url = "https://github.com/hoxnox/iptools"
    homepage =  "https://github.com/hoxnox/iptools"
    license = "https://github.com/hoxnox/iptools/blob/master/LICENSE"
    settings = "compiler"
    version = "0.4.4"

    def source(self):
        tools.get(**self.conan_data["sources"][self.version])

    def package(self):
        include_dir = "{}-{}/include/iptools".format(self.name, self.version)
        self.copy(pattern="*.hpp", dst="include/iptools", src=include_dir)

