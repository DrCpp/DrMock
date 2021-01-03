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

add_library(${PROJECT_NAME} SHARED
  drmock/mock/MethodCollection.cpp
  drmock/mock/StateObject.cpp
  drmock/test/Logger.cpp
  drmock/test/ILogger.cpp
  drmock/test/TestFailure.cpp
  drmock/test/Global.cpp
  drmock/test/TestObject.cpp
  drmock/test/FunctionInvoker.cpp
)

# This alias is required for the internal use of the `DrMock_module`
# macro.
add_library(${PROJECT_NAME}::Core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
  PUBLIC
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_compile_options(${PROJECT_NAME} PRIVATE
  -Wall -Werror -fPIC -pedantic -O2 -fdiagnostics-color=always
)
