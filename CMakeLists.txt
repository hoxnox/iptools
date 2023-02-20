# @author hoxnox <hoxnox@gmail.com>
# @date 20160516 09:09:55
# iptools cmake build script

cmake_minimum_required(VERSION 2.6.4)

########################################################################
# options

option(WITH_TESTS    "Build tests."  OFF)
option(WITH_DOCS     "Generate docs" OFF)
option(WITH_WARNINGS "Turn on some more checks" OFF)

########################################################################
# general

project(iptools)

set(iptools_VERSION_MAJOR 0)
set(iptools_VERSION_MINOR 5)
set(iptools_VERSION_PATCH 0)
# configuration header
configure_file(
	"${PROJECT_SOURCE_DIR}/src/iptools_config.cfg"
	"${PROJECT_SOURCE_DIR}/src/iptools_config.h"
	ESCAPE_QUOTES
)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}"
                      "${PROJECT_SOURCE_DIR}/cmake/Modules")

include_directories("${PROJECT_SOURCE_DIR}/include")
add_definitions("-std=c++11")

if(WITH_WARNINGS)
	# Add warning flags
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wformat=2 -Wnon-virtual-dtor -Wcast-qual")
	# Add warnings excluded
	# -Wparentheses is ignored because Logger operators cannot be isolated with 'pragma ignored'
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused -Wno-unused-parameter -Wno-parentheses")
endif()


########################################################################
# tests and docs

if(WITH_DOCS)
	add_subdirectory(doc)
endif()

if(WITH_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

########################################################################
# installation

set(CPACK_SET_DESTDIR ON)

#INSTALL(TARGETS
#	targetname
#	DESTINATION bin)
#INSTALL(FILES filepath DESTINATION bin
#	PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
#	            GROUP_EXECUTE GROUP_READ
#	            WORLD_READ WORLD_EXECUTE)
#INSTALL(DIRECTORY domedir DESTINATION share/${PROJECT_NAME})
SET(CPACK_PACKAGE_NAME iptools)
#if (WITH_SYSTEM_SOMELIB)
#	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libsomelib (>=1.1.1)")
#	SET(CPACK_RPM_PACKAGE_REQUIRES "libsomelib >= 1.1.1")
#endif()
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
SET(CPACK_PACKAGE_VENDOR "$uername$ <hoxnox@gmail.com>")
SET(CPACK_PACKAGE_CONTACT ${CPACK_PACKAGE_VENDOR})
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.markdown")
SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR ${iptools_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${iptools_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${iptools_VERSION_PATCH})
SET(CPACK_DEBIAN_PACKAGE_SECTION "misc")
SET(CPACK_RPM_PACKAGE_GROUP "Applications/System")
SET(CPACK_RPM_PACKAGE_LICENSE "MIT")
SET(CPACK_STRIP_FILES TRUE)
INCLUDE(CPack)
