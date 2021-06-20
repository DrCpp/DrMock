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


# Introduction to DrMock

**DrMock** is a C++17 testing and mocking framework
that offers unique features for mock object configuration.
It was first published in 2019 on GitHub.
Build requirements include libclang, CMake and Python.
**DrMock** offers compatability features for Qt5.
The framework consists of two components, which we will describe
briefly.

The _DrMockGenerator component_ is written in Python and used by
invoking **DrMock**'s CMake macros.
It parses C++ header files that contain interfaces, extracts their ASTs
and creates the source code of mock classes for the given interfaces.
The _core component_ is written in C++.
It offers a unit testing framework (error logger, test tables, wide
range of assertions, etc.) and a template framework for
configuring, controlling and verifying the behavior of the mock object's
methods.

An entirely unique feature of **DrMock** is its applicability to state
verification.
While mock objects (MOs) may be configured and verified in the
traditional manner of behavior behavior verification by setting fixed
return values for the MO's methods and verifying that each methods was
called the expected number of times, they may also be as _stubs_.
When used as stub, every MO has a current state, which may or may not
change when the MO's methods are called according to a state transition
table provided by the user.
The user may verify the correct behavior of the system under test by
checking that all MO's states have transitioned to the expected state
after execution.

The core component of **DrMock** is unit tested, the Python component is
tested functionally, and their source code is well documented.
New users will find multiple tutorials and sample projects that provide
a detailed outline of the framework's usage.

**DrMock** is licensed under GPL-3.0. Everyone is cordially invited to
contribute, provided they do so under the same license.
