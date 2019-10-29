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

include(../cmake/DrMockMacros.cmake)
include_directories(../src)

set (CMAKE_CXX_FLAGS 
  "-Wall -Werror -g -fPIC -pedantic -O0"
)

# Path to the generator script.
file(TO_CMAKE_PATH ${CMAKE_SOURCE_DIR}/python/DrMockGenerator pathToMocker)

macro(push_test module path)
  get_filename_component(name "${path}" NAME_WE)
  add_executable("${name}"
    "${path}"
  )
  target_link_libraries("${name}"
    ${module}
  )
  add_test(NAME "${name}" COMMAND "${name}")
endmacro()

#######################################
# Test Core.
#######################################

DrMockTest(TESTS
  Behavior.cpp
  Hash.cpp
  IsEqual.cpp
  Test.cpp
  Method.cpp
  IsTuplePackEqual.cpp
  MethodCollection.cpp
  BehaviorStack.cpp
  StateBehavior.cpp
  StateObject.cpp
)

#######################################
# Test generator sans Qt.
#######################################

# Tests for `python/drMockGenerator.py`. Errors in `mocker.py` are
# likely to cause a compiler error; the tests are there organised by
# likelihood of failure.

DrMockModule(
  TARGET DrMockMock
  GENERATOR ${pathToMocker}
  HEADERS 
    mocker/IVoidFunc.h 
    mocker/IFunc.h
    mocker/ITemplate.h
    mocker/IOperator.h
    mocker/IQualifier.h
    mocker/IVoidFuncOverload.h
    mocker/IFuncOverload.h
    mocker/ITemplateOverload.h
    mocker/IQualifierOverload.h
    mocker/ICurio.h
    mocker/ICurioTemplate.h
    mocker/IRvalue.h
    mocker/IFileExtension.hpp
    mocker/IAbstract.h
    mocker/IMultipleClasses.h
    mocker/IDerived.h
    mocker/ISharedState.h
    mocker/IVerifyState.h
    IExample.h
  INSTALLFLAG ON
  INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocker
)

DrMockTest(
  LIBS
    DrMockMock 
  TESTS 
    mocker/VoidFuncTest.cpp
    mocker/FuncTest.cpp
    mocker/TemplateTest.cpp
    mocker/OperatorTest.cpp
    mocker/QualifierTest.cpp
    mocker/VoidFuncOverloadTest.cpp
    mocker/FuncOverloadTest.cpp
    mocker/TemplateOverloadTest.cpp
    mocker/QualifierOverloadTest.cpp
    mocker/CurioTest.cpp
    mocker/CurioTemplateTest.cpp
    mocker/RvalueTest.cpp
    mocker/FileExtensionTest.cpp
    mocker/AbstractTest.cpp
    mocker/MultipleClassesTest.cpp
    mocker/DerivedTest.cpp
    mocker/SharedStateTest.cpp
    mocker/VerifyStateTest.cpp
    ExampleMockTest.cpp
)

#######################################
# Test generator with Qt.
#######################################

if (${Qt5_FOUND})
  DrMockModule(
    TARGET DrMockQtTests
    GENERATOR ${pathToMocker}
    HEADERS 
      mocker/IQObject.h
      mocker/ISlots.h
      mocker/ISignals.h
    QTMODULES
      Qt5::Core 
    INSTALLFLAG ON
  )

  DrMockTest(
    LIBS
      DrMockQtTests 
    TESTS
      mocker/QObjectTest.cpp
      mocker/SlotsTest.cpp
      mocker/SignalsTest.cpp
  )
endif()
