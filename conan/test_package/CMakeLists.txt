PROJECT(SnappyTest)
cmake_minimum_required(VERSION 2.7)

set(CMAKE_CXX_STANDARD 14)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

ADD_EXECUTABLE(test test.cpp)
TARGET_LINK_LIBRARIES(test ${CONAN_LIBS})
