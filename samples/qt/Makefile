# Copyright 2019 Ole Kliemann, Malte Kliemann
#
# This file is part of DrMock.
#
# DrMock is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# DrMock is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with DrMock.  If not, see <https://www.gnu.org/licenses/>.

# Discover operating system.
uname := $(shell uname -s)

# Get number of threads
ifeq ($(uname), Darwin)
	num_threads := $(shell sysctl -n hw.activecpu)
else  # Assuming Linux.
	num_threads := $(shell nproc)
endif

.PHONY: default
default:
	mkdir -p build
	cd build && cmake .. -DCMAKE_PREFIX_PATH="${DRMOCK_QT_PATH};../../build/install"
	cd build && make -j$(num_threads) && ctest --output-on-failure

.PHONY: clean
clean:
	rm -fr build && rm -fr prefix
