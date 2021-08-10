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
This sample demonstrates how to use **DrMock** to mock an interface
`IFoo` that inherits from `QObject` and uses the `Q_OBJECT` macro.

### Table of contents

* [Setup](#setup)
* [Mocking a QObject](#mocking-a-qobject)
* [Running the tests](#running-the-tests)
* [Event loops and the `DRTEST_USE_QT` macro](#event-loops-and-the-drtest_use_qt-macro)
* [The QObject-Interface-Implementation pattern](#the-qobject-interface-implementation-pattern)

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

### Requirements

This project requires an installation of **DrMock** in `prefix/` or the
`CMAKE_PREFIX_PATH` environment variable. If your installation of
**DrMock** is located elsewhere, you must change the value of
`CMAKE_PREFIX_PATH`.

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
bespecified using the keyword `QTMODULES`, as follows. **DrMock** will
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

Furthermore, in order to use `DrMockModule` with Qt, the `$DRMOCK_QT_PATH`
environment variable must be set (see [Building DrMock](../build.md) for
details).

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
  virtual void theSlot(const std::string&) = 0;

signals:
  void theSignal(const std::string&);
};
```

To demonstrate the mock object, two instances of `FooMock` are made
and `theSignal` of `foo` connected to the slot of `bar`.

```cpp
QObject::connect(
    foo.get(), &IFoo::theSignal,
    bar.get(), &IFoo::theSlot
  );
```

Before that, `bar` is instructed to expect a call of `theSlot` with
the argument `"foo"`. After `foo` emits `theSignal`, this may be
verified by `bar`:

```cpp
emit foo->theSignal("foo");

DRMOCK_ASSERT(bar->mock.verify());
```

The `emit foo->theSignal("foo");` looks a bit off. That's how we had to
do it prior to `DrMock<v0.3.0`. In `DrMock>=0.3.0`  the `emits`
configuration call is added to `Behavior` and `StateBehavior`. It can be
used to instruct a mock object to `emit` a signal:

```cpp
// emit foo->theSignal("foo");  // Old, boring and explicit.
foo->mock.theSlot().push()
    .emits<const std::string&>(&IFoo::theSignal, "foo")
    .expects("bar")
    .times(1);  // Optional.
foo->theSlot("bar");  // `emit foo->theSignal("foo")` happens here!
```

**Note.** As in the case above, you have to be explicit when passing
`const T&` instead of relying on template deduction. Only calling
`emits(&IFoo::theSignal, "foo")` will result in a
`deduced conflicting types for parameter 'SigArgs'` error.

**Note.** If you are using `DrMockGenerator` by hand (which we don't
recommend), you must set the `--qt` flag when invoking the mocker.
Otherwise, the `DRMOCK_USE_QT` macro will not be set, and emits will
raise an error.

## Running the tests

Do `make`. If everything checks out, this should return
```
    Start 1: FooTest
1/1 Test #1: FooTest ..........................   Passed    0.02 sec

100% tests passed, 0 tests failed out of 1

Total Test time (real) =   0.02 sec
```

## Event loops and the `DRTEST_USE_QT` macro

Some `QObject`s, such as `QEventLoop` require a `QApplication` to run in
the main thread of the program to function correctly. To run a
`QApplication` in the main thread during a `DRTEST_TEST`, define the
`DRTEST_USE_QT` macro before including `DrMock/Test.h`.

Let's take a look at the previous example, but this time, connect the
two `FooMock` instances via `Qt::QueuedConnection`:

```cpp
DRTEST_TEST(useQt)
{
  auto foo = std::make_shared<FooMock>();
  auto bar = std::make_shared<FooMock>();

  QObject::connect(
      foo.get(), &IFoo::theSignal,
      bar.get(), &IFoo::theSlot,
      Qt::QueuedConnection  // Connect via event loop.
    );

  bar->mock.theSlot().push().times(1);
  emit foo->theSignal();

  // ...
}
```

To process the emitted signal, we must use an event loop, something like
this:

```cpp
DRTEST_TEST(useQt)
{
  // ...

  QCoreApplication::processEvents();

  DRTEST_ASSERT(bar->mock.verify());
}
```

If you run this application (don't forget to include `<QCoreApplication>`),
you will receive the following error:

```
    Start 1: FooTest
1/1 Test #1: FooTest ..........................***Failed    0.02 sec
TEST   signalsAndSlots
*FAIL  signalsAndSlots (45): bar->mock.verify()
****************
1 FAILED

0% tests passed, 1 tests failed out of 1

Total Test time (real) =   0.02 sec

The following tests FAILED:
	  1 - FooTest (Failed)
Errors while running CTest
make: *** [default] Error 8
```

Due to the missing `QCoreApplication` in the main thread,
`processEvents()` fails to process `theSignal`, so `theSlot` never gets
called the expected number of times and `bar->mock.verify()` rightfully
returns `false`.

Now add `#define DRTEST_USE_QT` before `#include "DrMock/Test.h"` and
run the test again. The test should now succeed.
