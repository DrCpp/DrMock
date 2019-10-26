cmake_minimum_required(VERSION 3.10)

#######################################
# Project data.
#######################################

project(DrMockSampleMock)
set(CMAKE_CXX_STANDARD 17)

#######################################
# Dependencies.
#######################################

find_package(DrMock COMPONENTS Core REQUIRED)

#######################################
# Sources.
#######################################

add_subdirectory(src)

#######################################
# Testing.
#######################################

enable_testing()
add_subdirectory(tests)
