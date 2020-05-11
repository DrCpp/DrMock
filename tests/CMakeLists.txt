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

#######################################
# Manually setup macros, includes, paths.
#######################################

include(${CMAKE_SOURCE_DIR}/cmake/DrMockMacros.cmake)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Path to the generator script.
file(TO_CMAKE_PATH
  ${CMAKE_SOURCE_DIR}/python/DrMockGenerator
  pathToMocker
)

#######################################
# Test Core.
#######################################

DrMockTest(TESTS
  Behavior.cpp
  IsEqual.cpp
  Test.cpp
  Method.cpp
  IsTuplePackEqual.cpp
  MethodCollection.cpp
  BehaviorQueue.cpp
  StateBehavior.cpp
  StateObject.cpp
)

#######################################
# Test generator.
#######################################

add_subdirectory(mocker)

#######################################
# Test macros
#######################################

add_subdirectory(macros)
