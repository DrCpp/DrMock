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

# Put all headers/tests that can be used without Qt here!
set(regularHeaders
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
    IVerifyAll.h
    subdir/IBar.h
)

set(regularTests
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
    VerifyAllTest.cpp
)

if (${Qt5_FOUND})
    #######################################
    # Test generator with Qt.
    #######################################

    drmock_library(
        TARGET ${PROJECT_NAME}QtMock
        GENERATOR ${pathToMocker}
        HEADERS
            ${regularHeaders}
            IQObject.h
            ISlots.h
            ISignals.h
            IUseQt.h
            IEmit.h
            IDummy.h
            INotAQtObject.h
        QTMODULES
            Qt5::Core Qt5::Test
        OPTIONS -fPIC
    )
    target_compile_options(
        ${PROJECT_NAME}QtMock
        PRIVATE -Wall -Werror -g -fPIC -pedantic -O0
    )

    drmock_test(
        LIBS
            ${PROJECT_NAME}QtMock
        TESTS
            ${regularTests}
            QObjectTest.cpp
            SlotsTest.cpp
            SignalsTest.cpp
            UseQtTest.cpp
            EmitTest.cpp
            DummyTest.cpp
            MixingObjectsTest.cpp
    )
else()
    #######################################
    # Test generator without Qt.
    #######################################

    drmock_library(
        TARGET ${PROJECT_NAME}Mock
        GENERATOR ${pathToMocker}
        HEADERS ${regularHeaders}
    )
    target_compile_options(
        ${PROJECT_NAME}Mock
        PRIVATE -Wall -Werror -g -fPIC -pedantic -O0
    )

    drmock_test(
        LIBS ${PROJECT_NAME}Mock
        TESTS ${regularTests}
    )
endif()
