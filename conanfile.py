from nxtools import NxConanFile
from conans import tools

class IpToolsConan(NxConanFile):
    name = "iptools"
    description = "Header only library of IP utilities. Uses features of C++11."
    version = "0.4.2"
    options = {"noop":[True, False]}
    default_options = "noop=False"
    url = "https://github.com/hoxnox/iptools.git"
    license = "https://github.com/hoxnox/iptools/blob/master/LICENSE"
    settings = "os", "compiler", "build_type", "arch"

    def do_source(self):
        self.retrieve("2de58772d44c5b739d4521a513778065eceb8f0560ed6e23a9278a1269887ad7",
            [
                'vendor://hoxnox/iptools/iptools-{version}.tar.gz'.format(version=self.version),
                'https://github.com/hoxnox/iptools/archive/{version}.tar.gz'.format(version=self.version)
            ], "iptools-{v}.tar.gz".format(v = self.version))

    def do_package(self):
        tools.untargz("iptools-{v}.tar.gz".format(v=self.version))
        include_dir = "iptools-{v}/include/iptools".format(v=self.version)
        self.copy(pattern="*.hpp", dst="include/iptools", src=include_dir)

