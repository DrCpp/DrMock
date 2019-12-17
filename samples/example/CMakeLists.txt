# Copyright 2019 Ole Kliemann, Malte Kliemann
# 
# This file is part of DrMock.
# 
# DrMock is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# DrMock is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with DrMock.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.10)

#######################################
# Project data.
#######################################

project(DrMockSampleExamples)
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
