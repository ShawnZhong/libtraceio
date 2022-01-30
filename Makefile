lib_target := calltrace
sample_targets := trace_basic trace_test trace_leveldb db_bench

$(sample_targets): export CMAKE_ADDITIONAL_FLAGS := -DBUILD_SAMPLES=ON

.PHONY: $(lib_target) $(sample_targets)
$(lib_target) $(sample_targets):
	cmake -S . -B build -DCMAKE_BUILD_TYPE=release $(CMAKE_ARGS) $(CMAKE_ADDITIONAL_FLAGS)
	cmake --build build -j --target $@

.PHONY: sample all
sample: $(sample_targets)
all: $(lib_target) sample

.PHONY: clean
clean:
	rm -rf build*
