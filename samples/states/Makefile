default:
	mkdir -p build && cd build && cmake .. -DCMAKE_PREFIX_PATH=../../prefix && make -j10 && ctest --output-on-failure

clean:
	rm -fr build
