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
	cd python && make && cd ..
	mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH};${DRMOCK_QT_PATH}" -DCMAKE_INSTALL_PREFIX="../prefix"
	cd build && make -j$(num_threads) && ctest --output-on-failure

.PHONY: clean
clean:
	rm -fr build && rm -fr prefix

.PHONY: install
install:
	cd build && make install && cd ..
