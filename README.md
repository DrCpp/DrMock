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

# DrMock

**DrMock** is a testing and mocking framework for C++ designed for usage
with Qt5.

### Announcments

Release 0.1.0 is now available.

### Getting started

* A guide to building **DrMock** may be found [here](docs/build.md).
* **DrMock** sample projects and tutorials are found
  [here](docs/tutorial.md).

### Features

* Unit test framework
* Automated mock object source code generation
* State-based mock object behavior
* Qt5/QObject support 

Testing with **DrMock** looks like this:
```cpp
DRTEST_TEST(success)
{
  drmock::samples::Order order{"foo", 2};

  auto warehouse = std::make_shared<drmock::samples::WarehouseMock>();
  // Inform `warehouse` that it should expect an order of two units of
  // `"foo"` and should return `true` (indicating that those units are
  // available).
  warehouse->mock.remove().push()
      .expects("foo", 2)  // Expected arguments.
      .times(1)  // Expect **one** call only.
      .returns(true);  // Return value.

  order.fill(warehouse);
  // Check that `remove` was called with the correct arguments.
  DRTEST_ASSERT(warehouse->mock.verify());
  // Check that the return value of `filled` is correct.
  DRTEST_COMPARE(order.filled(), true);
}
```
For details, see [here](docs/tutorial.md).

## Requirements

### Supported platforms

**DrMock** is current supported on the following platforms:

* Linux
* macOS

### Build requirements

The basic requirements for building and running **DrMock** are:

* [CMake](https://cmake.org) (at least 3.10)

* A C++17-standard-compliant compiler

* [libclang](http://llvm.org/git/clang) (at least 6.0.0)

* [Python3](https://www.python.org) (at least 3.7)

The following Python3 packages are automatically installed during build:

* [Clang Python bindings](https://github.com/llvm-mirror/clang/tree/master/bindings/python) 
  (at least 6.0.0)

* [python3-setuptools](https://pypi.org/project/setuptools) (at least 41.4.0)

## Acknowledgments

During the configuration of **DrMock**'s build system, we have profited
greatly from P. Arias' 
[It's Time To Do CMake Right](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/) 
and D. Berner's 
[Cmake line by line - creating a header-only library](http://dominikberner.ch/cmake-interface-lib/).
