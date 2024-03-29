<!--
Copyright 2020 Ole Kliemann, Malte Kliemann

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

# DrMock 0.6.0

Released 2021/08/13

* Reorganize project structure along the lines of
  [vector-of-bool/pitchfork](gi thub.com/vector-of-bool/pitchfork)

* Move utility methods that are used in `mock/` and `test/` to
  `utility/`

* Remove python component from this repository (`drmock-generator`, as
  it is now called, will be installed using pip in the future)

* Clean up tutorials, add a specification for `drmock-generator`

* Remove broken (and superfluous) pkgconfig

* Add `USING_DRTEST` macro for removing `DRTEST_` prefix from test
  macros

* Place data funcs in `DRTEST_NAMESPACE`

* Clean up cmake/DrMockMacros.cmake

* Convert all CMake functions/macros from camelCase to snake_case

* Introduce `drmock_library2` function for list-based mock object code
  creation

* Update documentation for public `drmock::` classes

* Replace `MethodCollection` with `Controller`, a composition of a
  vector of `Method` objects and a `StateObject`

* Clean up `Controller`/`MethodCollection` error logging

* Remove `QObject` dependency from `Signal`

* Remove need for `DRMOCK_USE_QT` macro

* Remove Qt includes from `Method.h`

* Add `Variant` class to revert interface change of `expects` and
  `transition` which made using initializer-lists impossible


# DrMock 0.5.0

Released 2021/06/20

### Added/Changed

* Add `xfail` and `skil` tags to `drtest`

* Add floating point comparison macro `DRTEST_ASSERT_ALMOST_EQUAL` to
  `drtest`

* Add complex behaviors to `drmock`

* Add convenience function `addColumns`

* Add convenience functions `template<typename... Deriveds> expect`
  and `template<typename... Deriveds>`

* Change polymorphism behavior (`polymorphic` only applies to `expect`
  and `transition` calls following `polymorphic`)

* Remove `setIsEqual` from `AbstractBehavior`

### Fixed

* Fix issue #16

* Fix issue #15


# DrMock 0.4.2

Released 2021/05/24

### Fixed

* Fix issue #6

* Fix issue #10

  - In particular, remove forwarding ctor from `Signal` class


# DrMock 0.4.1

Released 2021/05/16 (re-released 2021/05/21)

### Added/Changed

* Create new utility module

### Fixed

* Fix issue #5

* Fix issue #7


# DrMock 0.4.0

Released 2020/08/16

### Added/Changed

* Add `DRTEST_ASSERT_DEATH` macro for death testing


# DrMock 0.3.0

Released 2020/07/05

### Added/Changed

* Add `emits` method that allows `Method` objects to emit signals when
  called (for details, see the tutorials). Implementing this feature
  meant introducing some significant changes (taken from git log):

  - All major class templates now have a new template parameter,
    the type of the parent mock object (MO). For every MO, the mocker passes
    to each Method object of the MO a pointer to the MO. This pointer is
    required for `Signal::invoke(Parent*)` [see below].

  - A new class template, `Signal`, represents such signal emissions.

  - Introduce the `--qt` flag for the `DrMockGenerator`, which, when set,
    `#define`s the DRMOCK_USE_QT macro in all mock source code. The
    **DrMock** macros automatically apply `--qt` if a test is linked
    against a Qt library. If `DRMOCK_USE_QT` is not set, calling
    `Signal::emit` raises an error.

  - Update `samples/qt`

* Throw if conflicts occur during mock object configuration 

### Fixed

* The wilcard state `"*"` is no longer ignored when other actions are
  defined, but rather serves as a catch-all (fallthru) state (the
  documentation regarding this has been clarified)


# DrMock 0.2.0

Released 2020/05/15

### Added/Changed

* Autodetect number of threads for compiling in Makefile

* Add convenience Makefile for building all sample projects

* Add tutorial for manually building **DrMock**

* Add pkg-config file

* Add `DRMOCK`, `DRMOCK_DUMMY` macros

  - Change order of includes in mock objects
    (so that `DRMOCK` is defined when including the interface header in
    mock object header)

* Allow applying `DRTEST_VERIFY_MOCK` to mock object (not just method objects)

  - Add `makeFormattedErrorString` virtual method to `IMethod` interface

  - Add `makeFormattedErrorString` method to `MethodCollection`
    (concatenates formatted error strings of collected method objects)

  - Add `makeFormattedErrorString` method to mock objects (returns
    formatted error string of method collection)

* Use `RESOURCES` parameter in `DrMockTest` to add resource files to
  test executables

* Add remark about QII pattern to docs

### Removed

* Disable verbose print from DrMockGenerator

### Fixed

* Add missing remark that `DRMOCK_QT_PATH` must be set when using
  `DrMockModule` with Qt5 modules to documentation.

* Apply do-while-false pattern to `DRTEST_VERIFY_MOCK`

* Fix formatting errors and typos in source/docs

* Fix transition table in rocket example

* Replace `python3.7` and `pip3.7` with `python` and `pip` and shifting
  the responsibility of managing the python versions to the user.

* Replace odd error message thrown when using `DrMockModule` with
  `QTMODULE` parameter but unset `DRMOCK_QT_PATH` environment variable.

* Throw error message if `DrMockTest` can't find files specified in `TESTS`


# DrMock 0.1.0

Released 2020/01/10

Official open-source release
