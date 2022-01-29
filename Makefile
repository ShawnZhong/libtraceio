trace: export build_dir := build
trace: export src_dir := .

sample_targets := basic_trace test_trace leveldb_trace
$(sample_targets): export build_dir := build-sample
$(sample_targets): export src_dir := sample

.PHONY: trace $(sample_targets)
trace $(sample_targets):
	cmake -S $(src_dir) -B $(build_dir) -DCMAKE_BUILD_TYPE=release $(CMAKE_ARGS)
	cmake --build $(build_dir) -j --target $@

.PHONY: sample all
sample: $(sample_targets)
all: trace sample

.PHONY: clean
clean:
	rm -rf build*
