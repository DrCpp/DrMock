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

### Added

* Build system:

  - Autodetect number of threads for compiling in Makefile

  - Convenience Makefile for building all sample projects

  - `make clean` directive to python Makefile

  - Tutorial for building **DrMock** without Makefile

  - pkg-config file

* Use `RESOURCES` parameter in `DrMockTest` to add resource files to
  test executables

### Changed

### Deprecated

???

### Removed

* Disable verbose print from DrMockGenerator

### Fixed

* Add remark that `DRMOCK_QT_PATH` must be set when using `DrMockModule`
  with Qt5 modules to documentation.

* Make python calls more robust by replacing `python3.7` and `pip3.7`
  with `python` and `pip` and shifting the responsibility of managing
  the python versions to the user.

* Replace odd error message thrown when using `DrMockModule` with
  `QTMODULE` parameter but unset `DRMOCK_QT_PATH` environment variable.

* Fix transition table in rocket example

# DrMock 0.1.0

Released 2020/01/10

Official open-source release
