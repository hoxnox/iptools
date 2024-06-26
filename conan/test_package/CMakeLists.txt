cmake_minimum_required(VERSION 3.15)
PROJECT(ConanTest)

set(CMAKE_CXX_STANDARD 17)

find_package(iptools REQUIRED)

add_executable(test_package test.cpp)
target_link_libraries(test_package iptools::iptools)
