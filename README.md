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

![Windows](https://github.com/DrCpp/drmock-generator/actions/workflows/windows.yml/badge.svg)
![Linux](https://github.com/DrCpp/drmock-generator/actions/workflows/linux.yml/badge.svg)
![macOS](https://github.com/DrCpp/drmock-generator/actions/workflows/macos.yml/badge.svg)


## Introduction to **DrMock**

**DrMock** is a C++17 testing and mocking framework for Windows, Linux
and macOS.

### Features

* Unit test framework
* On-the-fly mock object source code generation at compile time
* State machine-like mock objects
* Qt5 integration


### Example

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
      .transition("*",               "liftOff");  // "*" = catch-all

  // Check that the pad launches the rocket with at least one thruster enabled.
  drmock::samples::LaunchPad launch_pad{rocket};
  launch_pad.launch();
  DRTEST_ASSERT(rocket->mock.verifyState("liftOff"));
}
```

**Note.** Only the state of `rocket` after the test matters. It's fine
to keep toggeling the thrusters as long as at least one is enabled at
launch. **DrMock** allows tests to be designed without a particular
implementation in mind.


## Building **DrMock**

See [BUILD.md] for instructions.


## Documentation

* Tutorials:
  - [1. Testing](docs/tutorial/basic.md)
  - [2. Mocking](docs/tutorial/mock.md)
  - [3. State-machine mocking](docs/tutorial/states.md)
  - [4. Using **DrMock** with Qt5](docs/tutorial/qt.md)
  - [5. Death testing](docs/tutorial/death.md)

* The **DrMock** API is documented in [the source files](src/DrMock/mock);
  the docs are doxygen-compliant, but you must specify the Doxyfile
  yourself

* [Specification]

* [Cookbook], a collection of common design patterns for **DrMock**


## Contributing to **DrMock**

See [CONTRIBUTING.md].


## Acknowledgments

During the configuration of **DrMock**'s build system, we have profited
greatly from the following sources:

- P. Arias: [It's Time To Do CMake Right](https://pabloariasal.github.io/2018/02/19/its-time-to-do-cmake-right/)
- D. Berner: [Cmake line by line - creating a header-only library](http://dominikberner.ch/cmake-interface-lib/).

### Projects that use **DrMock**

* [DrAutomaton](https://github.com/DrCpp/DrAutomaton), Qt5-based cellular automaton library


<!-- Links -->

[Introduction to **DrMock**]: #introduction-to-drmock
[Building **DrMock**]: #building-drmock
[Documentation]: #documentation
[Contributing to **DrMock**]: #contributing-to-drmock
[Acknowledgments]: #acknowledgments

[BUILD.md]: BUILD.md
[Tutorial]: docs/tutorial.md
[Specification]: docs/spec.md
[CONTRIBUTING.md]: CONTRIBUTING.md
[Cookbook]: docs/cookbook.md
