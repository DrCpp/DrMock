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

**DrMock** is a testing and mocking framework for C++17 that offers unique
features for mock object configuration.

### Announcments

Release v0.5.x is now available. For details, see
[changelog](CHANGELOG.md).

### Getting started

* [Short introduction](docs/introduction.md) to **DrMock**

* [Build guide](docs/build.md)

* [Tutorials](docs/tutorial.md)

* [Cookbook](docs/cookbook.md), a collection of common design patterns for **DrMock**

### Features

* Unit test framework
* On-the-fly mock object source code generation
* Unique method of configuring mock objects by state
* Qt5 compatible

Testing and mocking with **DrMock** looks like this:
```cpp
DRTEST_TEST(launch)
{
  auto rocket = std::make_shared<drmock::samples::RocketMock>();

  // Define rocket's state behavior.
  rocket->mock.toggleLeftThruster().state()
                  // Source state    // Destination state   // Input value
      .transition("",                "leftThrusterOn",      true )
      .transition("leftThrusterOn",  "",                    false)
      .transition("rightThrusterOn", "allThrustersOn",      true )
      .transition("allThrustersOn",  "rightThrusterOn",     false);
  rocket->mock.toggleRightThruster().state()
      .transition("",                "rightThrusterOn",     true )
      .transition("rightThrusterOn", "",                    false)
      .transition("leftThrusterOn",  "allThrustersOn",      true )
      .transition("allThrustersOn",  "leftThrusterOn",      false);
  rocket->mock.launch().state()
      .transition("",                "failure")
      .transition("*",               "liftOff");

  // Check that the pad launches the rocket *after* enabling a thruster.
  drmock::samples::LaunchPad launch_pad{rocket};
  launch_pad.launch();
  DRTEST_ASSERT(rocket->mock.verifyState("liftOff"));
}
```
Note that it doesn't matter how many times the thrusters are switched on
and off, only the state of `rocket` after the test matters. **DrMock**
allows tests to be designed without a particular implementation in mind.

For details, see [here](docs/samples/states.md).

## Requirements

### Supported platforms

**DrMock** is current supported on the following platforms:

* Linux
* macOS

### Build requirements

The basic requirements for building and running **DrMock** are:

* [CMake](https://cmake.org) (at least 3.17)

* A C++17-standard-compliant compiler

* [libclang](http://llvm.org/git/clang) (at least 6.0.0)

* [Python3.7](https://www.python.org)

The following Python3 packages are automatically installed during build:

* [Clang Python bindings](https://github.com/llvm-mirror/clang/tree/master/bindings/python)
  (at least 6.0.0)

* [python3-setuptools](https://pypi.org/project/setuptools) (at least 41.4.0)

* [wheel](https://pythonwheels.com) (at least 0.32.3)

## Contributing to **DrMock**

See [CONTRIBUTING.md](CONTRIBUTING.md).

## Projects that use **DrMock**

* [DrAutomaton](https://github.com/DrCpp/DrAutomaton), Qt-based cellular automaton library

## Acknowledgments

During the configuration of **DrMock**'s build system, we have profited
greatly from P. Arias'
[It's Time To Do CMake Right](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/)
and D. Berner's
[Cmake line by line - creating a header-only library](http://dominikberner.ch/cmake-interface-lib/).
