lib_target := calltrace
sample_targets := trace_basic trace_test trace_leveldb db_bench

$(lib_target): export build_dir := build
$(lib_target): export src_dir := .

$(sample_targets): export build_dir := build-sample
$(sample_targets): export src_dir := sample

.PHONY: $(lib_target) $(sample_targets)
$(lib_target) $(sample_targets):
	cmake -S $(src_dir) -B $(build_dir) -DCMAKE_BUILD_TYPE=release $(CMAKE_ARGS)
	cmake --build $(build_dir) -j --target $@

.PHONY: sample all
sample: $(sample_targets)
all: $(lib_target) sample

.PHONY: clean
clean:
	rm -rf build*
