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
pip3.7 install [--user] python/dist/DrMockGenerator-[version]-py3-none-any.whl
[--target TARGET]
```
where `TARGET` is the target directory for the installation and brackets
denote optional arguments, or by copying the contents of `python/build`
to a convenient location.

## Building with support for Qt

If you wish to mock Qt5's `Q_OBJECT`s, set the environment variable
`$DRMOCK_QT_PATH` equal to the location of the Qt library before
following the steps above. Example:

```
export DRMOCK_QT_PATH="$HOME/Qt/5.13.1/clang_64"
```

## Fetching Dependencies

Some notes on fetching dependencies. 

### CMake

On some Linux systems, CMake 3.13 might not be available via the package
manager you're using (this seems to be the case with Ubuntu 18.04). It's
not difficult to build from source, following
[https://cmake.org/install/](https://cmake.org/install/).

We've noticed that on a mint Ubuntu installation, the following error
will occur when doing `./bootstrap`: [CMake not able to find OpenSSL
library](https://stackoverflow.com/questions/16248775/cmake-not-able-to-find-openssl-library),
which we were able to solve by installing `openssl-dev`:
```
sudo apt-get install openssl-dev
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
