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
the contents of that folder wherever you please.

The `make` call also configures and builds the python script
`DrMockGenerator` and its dependencies. This script is required for
generating the source code of mock objects. It may be installed by doing

```
pip3 install python/dist/DrMockGenerator-[version]-py3-none-any.whl
[--target TARGET]
```

where `TARGET` is the target directory for the installation, or by
copying the contents of `python/build` to a convenient location.

### Building with support for Qt

If you wish to mock Qt5's `Q_OBJECT`s, set the environment variable
`$DRMOCK_QT_PATH` equal to the location of the Qt library before
following the steps above. Example:

```
export DRMOCK_QT_PATH="$HOME/Qt/5.13.1/clang_64"
```
