cmake_targets := test_trace leveldb_trace

.PHONY: release debug
release debug:
	cmake -S . -B build-$@ -DCMAKE_BUILD_TYPE=$@ $(CMAKE_ARGS)
	cmake --build build-$@ -j --target $(cmake_targets)

.PHONY: clean
clean:
	rm -rf build-*
