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
	cd build && cmake .. -DCMAKE_INSTALL_PREFIX=install -DCMAKE_PREFIX_PATH=${DRMOCK_QT_PATH}
	cd build && make -j$(num_threads) && ctest --output-on-failure

.PHONY: clean
clean:
	rm -fr build
	cd samples && make clean

.PHONY: install
install:
	cd build && make install && cd ..
