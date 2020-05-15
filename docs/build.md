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

# Building DrMock

## Requirements for building and using DrMock

Build requirements and supported platforms are found in the
[README.md](../README.md). Using and building **DrMock** requires that
`python` and `pip` are >= 3.7.0 (we suggest you use
[pyenv](https://github.com/pyenv/pyenv) to manage your python versions;
see below for details).

## Building

**DrMock** may be configured manually, or using the delivered Makefiles.

### Using the Makefile

In the source directory, do `make`, then `make install`. This will
install the **DrMock** cmake package into `{SOURCE_DIR}/prefix`. Move
the contents of that folder wherever you please. For example,
```
rsync -a prefix/ /usr/local
```

The `make` call also configures and builds the python script
`DrMockGenerator` and its dependencies. This script is required for
generating the source code of mock objects. It may be installed by doing
```
pip install [--user] python/dist/DrMockGenerator-[version]-py3-none-any.whl [--target TARGET]
```
where `TARGET` is the target directory for the installation and brackets
denote optional arguments, or by copying the contents of `python/build`
to a convenient location.

### Manually using CMake

If you do not wish to use the presupplied Makefile, you can use CMake
manually to install **DrMock**. Configure the C++ component _without_
building it:
```
cmake .
```
Don't forget to set `CMAKE_PREFIX_PATH` if necessary. Then install the
python component:
```
cd python
python3.7 setup.py bdist_wheel
cd dist
pip3.7 install [--user] DrMockGenerator-[version]-py3-none-any.whl [--target TARGET]
cd ..
cd ..
```
where `TARGET` is the target directory for the installation and brackets
denote optional arguments.

_Then_ install the C++ component:
```
make && make install
```

This order is necessary, as running the CMake configuration writes the
location of libclang to `python/mocker/mocker.cfg`, which the python
component requires for testing, and the python component is required
to be fully functional when building the C++ component.

## Building with Qt

If you wish to mock Qt5's `Q_OBJECT`s, set the environment variable
`$DRMOCK_QT_PATH` equal to the location of the Qt library before
following the steps above. Example:
```
export DRMOCK_QT_PATH="$HOME/Qt/5.13.1/clang_64"
```
If you're using the Makefile for building, `${DRMOCK_QT_PATH}` will be
used to set the `CMAKE_PREFIX_PATH`. If you're building manually and
haven't already, you will also have to add `${DRMOCK_QT_PATH}` to your
prefix path by using a CMake directive:
```
cmake. -D CMAKE_PREFIX_PATH=$DRMOCK_QT_PATH
```

**Note.** `$DRMOCK_QT_PATH` must be set in order to use **DrMock** with
  Qt.

## Fetching Dependencies

Some notes on fetching dependencies.

### CMake

On some Linux systems, CMake 3.13 might not be available via the package
manager you're using (this seems to be the case with Ubuntu 18.04). It's
not difficult to build CMake from source, following
[https://cmake.org/install/ ](https://cmake.org/install/).

We've noticed that on a mint Ubuntu installation, the following error
will occur when doing `./bootstrap`: [CMake not able to find OpenSSL
library](https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library),
which we were able to solve by installing `openssl-dev`:
```
sudo apt-get install libssl-dev
```
(or similar for other package managers).

### libclang

To satisfy the libclang dependency, do
```
sudo apt-get install libclang-6.0-dev
```
(or similar for other package managers); it is not sufficient to install
the `libclang1-6.0` package.

## Troubleshooting

### `libgl` missing

You might encounter the following on Linux when linking against
`Qt5::Widgets`:
```
${DRMOCK_QT_PATH}/lib/cmake/Qt5Gui/Qt5GuiConfigExtras.cmake:9 (message):
  Failed to find "GL/gl.h" in "/usr/include/libdrm".
```
The solution is to install `libgl-dev`:
```
sudo apt-get install libgl-dev
```
(or similar for other package managers).

### `python` versions

On many systems, `python` will still point to Python 2.7.x, which has
reached the end of its life on 01/01/2020 (as of January 2020, macOS
Catalina and Ubuntu 18.04 are examples of this).
Using this configuration **DrMock** requires `python` to point to Python >= 3.7.0.
The proper way to do this is to use the aforementioned
[pyenv](https://github.com/pyenv/pyenv).
