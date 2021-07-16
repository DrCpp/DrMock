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

# Manually setup macros, includes, paths.
include(${CMAKE_SOURCE_DIR}/cmake/DrMockMacros.cmake)
include_directories(${CMAKE_SOURCE_DIR}/src)

# Test Core.
drmock_test(TESTS
    Behavior.cpp
    BehaviorQueue.cpp
    MatchPack.cpp
    IsEqual.cpp
    Method.cpp
    MethodCollection.cpp
    StateBehavior.cpp
    StateObject.cpp
    Test.cpp
    TypeTraits.cpp
    MakeTupleOfMatchers.cpp
)

# Test Signal.
if (${Qt5_FOUND})
    add_library(DrMockSignalDummy SHARED Dummy.cpp)
    target_link_libraries(DrMockSignalDummy Qt5::Core Qt5::Test)
    drmock_test(
        TESTS Signal.cpp
        LIBS DrMockSignalDummy
    )
endif()

add_subdirectory(integration)
