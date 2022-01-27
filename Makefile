release debug:
	cmake -S . -B build-$@ -DCMAKE_BUILD_TYPE=$@ $(CMAKE_ARGS)
	cmake --build build-$@ -j

clean:
	rm -rf build-*
