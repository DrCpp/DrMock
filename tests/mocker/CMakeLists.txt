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
# Test generator sans Qt.
#######################################

# Tests for `python/drMockGenerator.py`. Errors in `mocker.py` are
# likely to cause a compiler error; the tests are there organised by
# likelihood of failure.

# Include one of the subdirs. `IncludeTest.cpp` checks that this
# inclusion is passed down to the generator.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/subdirInc)

DrMockModule(
  TARGET DrMockMock
  GENERATOR ${pathToMocker}
  HEADERS 
    IVoidFunc.h 
    IFunc.h
    ITemplate.h
    IOperator.h
    IQualifier.h
    IVoidFuncOverload.h
    IFuncOverload.h
    ITemplateOverload.h
    IQualifierOverload.h
    ICurio.h
    ICurioTemplate.h
    IRvalue.h
    IFileExtension.hpp
    IAbstract.h
    IMultipleClasses.h
    IInclude.h
    IDerived.h
    ISharedState.h
    IVerifyState.h
    IExample.h
    subdir/IBar.h
  INSTALLFLAG ON
)

DrMockTest(
  LIBS
    DrMockMock 
  TESTS 
    VoidFuncTest.cpp
    FuncTest.cpp
    TemplateTest.cpp
    OperatorTest.cpp
    QualifierTest.cpp
    VoidFuncOverloadTest.cpp
    FuncOverloadTest.cpp
    TemplateOverloadTest.cpp
    QualifierOverloadTest.cpp
    CurioTest.cpp
    CurioTemplateTest.cpp
    RvalueTest.cpp
    FileExtensionTest.cpp
    AbstractTest.cpp
    MultipleClassesTest.cpp
    IncludeTest.cpp
    SubdirTest.cpp
    DerivedTest.cpp
    SharedStateTest.cpp
    VerifyStateTest.cpp
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
      IQObject.h
      ISlots.h
      ISignals.h
    QTMODULES
      Qt5::Core 
    INSTALLFLAG ON
  )

  DrMockTest(
    LIBS
      DrMockQtTests 
    TESTS
      QObjectTest.cpp
      SlotsTest.cpp
      SignalsTest.cpp
  )
endif()
