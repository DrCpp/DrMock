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

# DrMock 0.2.0

Released 2020/??/??

### Added/Changed:

* Autodetect number of threads for compiling in Makefile

* Add convenience Makefile for building all sample projects

* Add tutorial for manually building **DrMock**

* Add pkg-config file

* Add DRMOCK, DRMOCK_DUMMY macros

  - Change order of includes in mock objects
    (so that `DRMOCK` is defined when including the interface header in
    mock object header)

* Allow applying `DRTEST_VERIFY_MOCK` to mock object (not just method objects)

  - Add `makeFormattedErrorString` virtual method to `IMethod` interface

  - Add `makeFormattedErrorString` method to `MethodCollection`
    (concatenates formatted error strings of collected method objects)

  - Add `makeFormattedErrorString` method to mock objects (returns
    formatted error string of method collection)

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

* Throw error message if `DrMockTest` can't find files specified in `TESTS`.

# DrMock 0.1.0

Released 2020/01/10

Official open-source release
