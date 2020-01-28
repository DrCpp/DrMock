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

Release v0.2.x is now available. For details, see
[changelog](CHANGELOG.md).

### Getting started

* A short introduction to **DrMock** may be found [here](docs/introduction.md)

* A guide to building **DrMock** may be found [here](docs/build.md)

* **DrMock** sample projects and tutorials may be found [here](docs/tutorial.md)

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
                  // Source state    // Destination state  // Input value
      .transition("",                "leftThrusterOn",     true )
      .transition("leftThrusterOn",  "",                   false)
      .transition("rightThrusterOn", "allThrustersOn",     true )
      .transition("allThrustersOn",  "rightThrusterOn",    false);
  rocket->mock.toggleRightThruster().state()
      .transition("",                "rightThrusterOn",    true )
      .transition("rightThrusterOn", "",                   false)
      .transition("leftThrusterOn",  "allThrustersOn",     true )
      .transition("allThrusterOn",   "rightThrusterOn",    false);
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

* [CMake](https://cmake.org) (at least 3.13)

* A C++17-standard-compliant compiler

* [libclang](http://llvm.org/git/clang) (at least 6.0.0)

* [Python3.7](https://www.python.org)

The following Python3 packages are automatically installed during build:

* [Clang Python bindings](https://github.com/llvm-mirror/clang/tree/master/bindings/python) 
  (at least 6.0.0)

* [python3-setuptools](https://pypi.org/project/setuptools) (at least 41.4.0)

* [wheel](https://pythonwheels.com) (at least 0.32.3)

## Contributing to **DrMock**

You are cordially invited to contribute to **DrMock** by reporting and
fixing bugs, and by proposing and implementing new features. 

To report bugs or request a feature, raise an issue in this repository.
Make a pull request if you wish to fix a bug or implement a new feature.

When making a pull request, please ensure the code is properly unit
tested, and that all tests pass. If changes are made to the CMake or
Python component, please mind that **DrMock** succeeds to build and
passes the unit tests on _all_ supported platforms.

### Licensing

All contributions will be issued under the GPL 3.0 license. All new
files must include a GPL 3.0 license stub, including your authorship
copyright.

### Code style

When contributing code, please follow the style of existing **DrMock**
source code.

## Acknowledgments

During the configuration of **DrMock**'s build system, we have profited
greatly from P. Arias' 
[It's Time To Do CMake Right](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/) 
and D. Berner's 
[Cmake line by line - creating a header-only library](http://dominikberner.ch/cmake-interface-lib/).
