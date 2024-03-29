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

# Check that the resources parameter works correctly.
drmock_test(
    TESTS
        ResourcesFile.cpp
    RESOURCES
        resources/Foo.cpp
)

# Integration tests. Errors in the generator are likely to cause a
# compiler error; the tests are there organised by likelihood of
# failure.

# Include one of the subdirs. `IncludeTest.cpp` checks that this
# inclusion is passed down to the generator.
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/subdir_include)

# Default tests that can run without Qt.
drmock_library(
    TARGET ${PROJECT_NAME}Mock
    HEADERS
        IVoidFunc.h
        IFunc.h
        IFuncNotAbstract.h
        INoDefaultCtor.h
        ITemplate.h
        IAccessSpec.h
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
        ICopyAndMove.h
        ISharedState.h
        IVerifyState.h
        IExample.h
        IVerifyAll.h
        subdir/ISubdir.h
)

if (NOT WIN32)
    set(flags -Werror -g -pedantic -O0 -fPIC)
    target_compile_options(${PROJECT_NAME}Mock PRIVATE ${flags})
endif()
# No fPIC for Windows!
drmock_library(
    TARGET ${PROJECT_NAME}MockArgs
    HEADERS IArgs.h
    OPTIONS --namespace ns --controller ctrl
    FLAGS ${flags}
)  # Need a separate library to test OPTIONS and FLAGS keyword

drmock_test(
    LIBS
        ${PROJECT_NAME}Mock
        ${PROJECT_NAME}MockArgs
    TESTS
        VoidFuncTest.cpp
        ArgsTest.cpp
        FuncTest.cpp
        FuncNotAbstractTest.cpp
        NoDefaultCtorTest.cpp
        TemplateTest.cpp
        AccessSpecTest.cpp
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
        CopyAndMoveTest.cpp
        SharedStateTest.cpp
        VerifyStateTest.cpp
        ExampleMockTest.cpp
        VerifyAllTest.cpp
)

if (${Qt5_FOUND})
    drmock_library(
        TARGET ${PROJECT_NAME}QtMock
        HEADERS
            IQObject.h
            ISlots.h
            ISignals.h
            IUseQt.h
            IEmit.h
            IDummy.h
            INotAQtObject.h
        QTMODULES
            Qt5::Core Qt5::Test
        FLAGS ${flags}
    )
    if (NOT WIN32)
        target_compile_options(${PROJECT_NAME}QtMock PRIVATE ${flags})
    endif()
    drmock_test(
        LIBS
            ${PROJECT_NAME}QtMock
        TESTS
            QObjectTest.cpp
            SlotsTest.cpp
            SignalsTest.cpp
            UseQtTest.cpp
            EmitTest.cpp
            DummyTest.cpp
            MixingObjectsTest.cpp
    )
endif()
