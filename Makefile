trace all sample_trace test_trace leveldb_trace:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=release $(CMAKE_ARGS)
	cmake --build build -j --target $@

.PHONY: clean
clean:
	rm -rf build
