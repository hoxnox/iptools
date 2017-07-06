from nxtools import NxConanFile
from conans import tools, CMake

class IpToolsConan(NxConanFile):
    name = "iptools"
    description = "Header only library of IP utilities. Uses features of C++11."
    version = "0.3.2"
    options = {"noop":[True, False]}
    default_options = "noop=False"
    url = "https://github.com/hoxnox/iptools.git"
    license = "https://github.com/hoxnox/iptools/blob/master/LICENSE"
    settings = "os", "compiler", "build_type", "arch"

    def do_source(self):
        self.retrieve("3a64f5f0f75864ee582e3a14b03114eb50f3d7510d1f0010687a2e5e41ec299d", # TOFIX
            [
                'vendor://hoxnox/iptools/iptools-{version}.tar.gz'.format(version=self.version),
                'https://github.com/hoxnox/iptools/archive/{version}.tar.gz'.format(version=self.version)
            ], "iptools-{v}.tar.gz".format(v = self.version))

    def do_package(self):
        tools.untargz("iptools-{v}.tar.gz".format(v=self.version))
        include_dir = "iptools-{v}/include/iptools".format(v=self.version)
        self.copy(pattern="*.hpp", dst="include/iptools", src=include_dir)

