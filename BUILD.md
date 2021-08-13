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


## Dependencies

### Supported platforms

**DrMock** is current supported on the following platforms:

* Windows
* Linux
* macOS


### Installing dependencies

Go through the following steps to ensure that all dependencies are satisfied:

1. Install `cmake` (minimum 3.17):

```
choco install cmake          (Windows)
sudo apt-get install cmake   (Linux)
brew install cmake           (macOS)
```

2. Install `libclang` (minimum 6.0.0):

```
choco install llvm                      (Windows)
sudo apt-get install libclang-6.0-dev   (Linux)
```

On macOS, `libclang` is installed by default.
Note that, it is not sufficient to install the `libclang1-6.0` package
on Linux. Installing later versions of `libclang` should be fine.

3. Install `drmock-generator`:

```
pip install drmock-generator
```


## Building

### Manually

To build and install **DrMock**, run

```
cmake .
make
make install
```

Add whatever `-DCMAKE` directives you wish. Don't forget to set
`CMAKE_PREFIX_PATH` if necessary.

On Windows, CMake may fail to detect the correct `libclang.dll`. If that
is the case, you should set the `CLANG_LIBRARY_FILE` environment
variable to the path of the `libclang.dll`. When using `choco`, then
`C:\Program Files\LLVM\bin\libclang.dll` is usually correct.


### Using the Makefile

To build **DrMock** using the presupplied `Makefile`, proceed as
follows: In the source directory, do `make`, then `make install`. This
will install the **DrMock** cmake package into `build/install`. Move the
contents of that folder wherever you please. For example,
```
rsync -a build/install/ /usr/local
```


## Building with Qt

If you wish to mock `Q_OBJECT`s, set the environment variable
`$DRMOCK_QT_PATH` equal to the location of the Qt library before
following the steps above. Example:
```
export DRMOCK_QT_PATH="$HOME/Qt/5.13.1/clang_64"
```
If you're using the `Makefile` for building, `${DRMOCK_QT_PATH}` will be
used to set the `CMAKE_PREFIX_PATH`. If you're building manually and
haven't already, you will also have to add `${DRMOCK_QT_PATH}` to your
prefix path by using a CMake directive:
```
cmake. -D CMAKE_PREFIX_PATH=$DRMOCK_QT_PATH
```

**Note.** `$DRMOCK_QT_PATH` must be set in order to use **DrMock** with
  Qt.


## Troubleshooting

### Installing CMake on Linux

On some Linux systems, CMake 3.17 might not be available via `apt-get`
(this seems to be the case with Ubuntu 18.04). It's not difficult to
build CMake from source, following
[https://cmake.org/install/ ](https://cmake.org/install/).

We've noticed that on a mint Ubuntu 18.04 installation, the following error
occurs during `./bootstrap` (see [CMake not able to find OpenSSL
library](https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library)),
which we were able to solve by installing `openssl-dev`:
```
sudo apt-get install libssl-dev
```
(or similar for other package managers).


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


### Old versions of Python

On many systems, `python` will still point to Python 2.7.x, which has
reached the end of its life on 01/01/2020 (as of January 2020, macOS
Catalina and Ubuntu 18.04 are examples of this).
Using this configuration **DrMock** requires `python` to point to Python (minimum 3.7.0).
We recommend using [pyenv](https://github.com/pyenv/pyenv) for this.
