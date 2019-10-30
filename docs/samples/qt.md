<!-- 
Copyright 2019 Ole Kliemann, Malte Kliemann

This file is part of DrMock.

DrMock is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

DrMock is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with DrMock.  If not, see <https://www.gnu.org/licenses/>.
-->

# samples/qt

### Table of contents

* [Setup](#setup)
* [Mocking a QObject](#mocking-a-qobject)
* [Running the tests](#running-the-tests)

### Project structure

```
samples/qt
│   CMakeLists.txt
│   Makefile
│
└───src
│   │   CMakeLists.txt
│   │   IFoo.h
│   
└───tests
    │   CMakeLists.txt
    │   FooTest.cpp
```

This sample demonstrates how to use **DrMock** to mock an interface
`IFoo` that inherits from `QObject` and uses the `Q_OBJECT` macro. 

## Setup

Let's take a look at the changes make to `CMakeLists.txt`. 

```cmake
# samples/qt/CMakeLists.txt

# ...

find_package(DrMock COMPONENTS Core REQUIRED)
DrMockEnableQt()
find_package(Qt5 COMPONENTS Core REQUIRED)

# ...
```

To enable Qt for **DrMock**, the `DrMockEnableQt()` macro must be called
after finding the **DrMock** package. This macro enables `CMAKE_AUTOMOC`
and a CMake policy that allows mocking generated source code. Of course,
one must also find the Qt5 package.

When calling `DrMockModule`, the required Qt5 modules must now
bespecified using the keyword `QTMODULES`, as follows.  **DrMock** will
automatically link `TARGET` to the required libraries.

```cmake
DrMockModule(
  TARGET DrMockSampleQtMocked
  generator ${pathToMocker}
  QTMODULES
    Qt5::Widget
    # ...
  HEADERS
    IFoo.h
)
```

The `DrMockTest` call requires no changes for the use of Qt.

## Mocking a QObject

A class that is derived from `QObject`<sup>3</sup> and holds the `Q_OBJECT` macro in
the private section of its body may be mocked if it satisfies the
following rules:

* The only declarations in the interface shall be public methods, public
  slots, signals and type alias (template) declarations.

* All methods shall be declared pure virtual, with the exception of the
  signals.

* The interface shall not contain any conversion functions.

* If an operator is defined in the interface, the interface shall not
  have a method called `operator[SYMBOL]`, where `[SYMBOL]` is
  determined by the operator's symbol according to the table below.

* None of the interface's method shall be a volatile qualified method
  or a method with volatile qualified parameters.

<sup>3</sup>: Note that condition is satisfied if, for instance, the class is
  derived from `QWidget`, which in turn is derived from `QObject`.

The interface `IFoo` inhertis from `QWidget`, has a pure virtual slot
and a signal.

```cpp
class IFoo : public QWidget
{
  Q_OBJECT

public:
  virtual ~IFoo() = default;

public slots:
  virtual void someSlot(const std::string&) = 0;

signals:
  void someSignal(const std::string&);
};
```

To demonstrate the mock object, two instances of `FooMock` are made
and `someSignal` of `foo1` connected to the slot of `foo2`.

```cpp
QObject::connect(
    foo1.get(), &IFoo::someSignal,
    foo2.get(), &IFoo::someSlot
  );
```

Before that, `foo2` is instructed to expect a call of `someSlot` with
the argument `"foo"`. After `foo1` emits `someSignal`, this may be
verified by `foo2`:

```cpp
emit foo1->someSignal("foo");

DRMOCK_ASSERT(foo2->mock.verify());
```

## Running the tests

Do `make`. If everything checks out, this should return
```
    Start 1: FooTest
1/1 Test #1: FooTest ..........................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.02 sec
```
