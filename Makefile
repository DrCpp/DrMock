default:
	mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH=${DRMOCK_QT_PATH}
	cd python && make && python3 setup.py bdist_wheel && cd ..
	mkdir -p build && cd build && make -j10 && ctest --output-on-failure

clean:
	rm -fr build

install:
	cd build && make install && cd ..
