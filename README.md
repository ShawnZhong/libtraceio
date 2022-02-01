# libtraceio

A library for tracing I/O system calls and function call graph

## Usage

1. (Optional but recommended) Add the following flags to the compiler's command
   line:

   ```
   -finstrument-functions \
   -finstrument-functions-exclude-file-list=/usr/include \
   -rdynamic -g
   ```

   <details>
   <summary>What are these flags and why are they optional? </summary>

    - When the flag `-finstrument-functions` is set, compiler generates calls
      to `__cyg_profile_func_enter` on function enter
      and `__cyg_profile_func_exit` on function exit for all functions
      (including inlined ones).

      The signatures of the two instrumentation functions are shown below:

      ```cpp
      void __cyg_profile_func_enter(void *this_fn, void *call_site);
      void __cyg_profile_func_exit(void *this_fn, void *call_site);
      ```

      The two arguments are the address of the function being called and the
      address of the call site.

      Our library implements these functions to gather the function call trace
      and generate the backtrace for I/O system calls. If the flag is not set,
      then our library cannot gather the call trace (i.e., `TRACE_LOG_FN=1` will
      have no effect). The backtrace can still be generated using
      [`backtrace(3)`](https://man7.org/linux/man-pages/man3/backtrace.3.html).
      Since `backtrace` generates trace using the stack frame, some functions
      (i.e., inlined functions) might not be shown due to omission of the frame
      pointers (See `-fomit-frame-pointer` enabled at `-O1` and higher.).

    - The `-finstrument-functions-exclude-file-list` flag specifies a list of
      files to exclude from the instrumentation. If the flag is not set, then
      the call traces for all files are generated, including those in the
      standard library. For example, a single construction and destruction
      of  `std::string` can generate 70 lines of call trace.

    - The `-Wl,--export-dynamic` flag adds all symbols to the dynamic symbol
      table in the `.dynsym` section.

      For function name resolution, given a function address, we use first
      [`dladdr(3)`](https://man7.org/linux/man-pages/man3/dladdr.3.html) to
      obtain the function name if it is present in the dynamic symbol table.
      This method does not work for static functions, since they are not
      exported by the flag. If verbose mode is enabled (
      i.e., `TRACE_VERBOSE_FN=1` or `TRACE_VERBOSE_IO=1`), then we also
      use [`libbfd`](https://en.wikipedia.org/wiki/Binary_File_Descriptor_library)
      to obtain the function name, source file name, and line number by reading
      the ELF file.

      If the flag is not set and/or verbosity is not enabled, then you may see
      lines like `0x11a0 in trace_basic` in the trace output. One can manually
      use `nm` to resolve the function name:

      ```shell
      nm -C trace_basic | grep 11a0 # prints `00000000000011a0 T main`
      ```

    - The `-g` flag enables the generation of debugging information, so that the
      source file and line number can be obtained.

      </details>

2. Run your program with `libtraceio.so` loaded.

## Example

Compile [sample/basic.cpp](sample/basic.cpp) and run it with `libtraceio.so`:

```shell
g++ sample/basic.cpp -g -O3 -finstrument-functions -finstrument-functions-exclude-file-list=/usr/include -rdynamic
make # compile libtraceio.so
TRACE_LOG_FN=1 TRACE_VERBOSE_FN=1 LD_PRELOAD=./build/libtraceio.so ./a.out
```

Output:

```cpp
> main(...) in /home/szhong/IOTrace/sample/basic.cpp:18
 > some_namespace::inline_fn() in /home/szhong/IOTrace/sample/basic.cpp:15
  > some_namespace::static_fn(int, int) in /home/szhong/IOTrace/sample/basic.cpp:11
   > some_namespace::io_fn(char const*) in /home/szhong/IOTrace/sample/basic.cpp:5
    > open("/dev/null", 0) = 4 in 6606ns
     = backtraces:
     = [3] some_namespace::io_fn(char const*) in /home/szhong/IOTrace/sample/basic.cpp:5
     = [2] some_namespace::static_fn(int, int) in /home/szhong/IOTrace/sample/basic.cpp:11
     = [1] some_namespace::inline_fn() in /home/szhong/IOTrace/sample/basic.cpp:15
     = [0] main(...) in /home/szhong/IOTrace/sample/basic.cpp:18
    < open(...)
    > read(4, 0x7ffcce260f10, 0) = 0 in 308ns
     = backtraces:
     = [3] some_namespace::io_fn(char const*) in /home/szhong/IOTrace/sample/basic.cpp:5
     = [2] some_namespace::static_fn(int, int) in /home/szhong/IOTrace/sample/basic.cpp:11
     = [1] some_namespace::inline_fn() in /home/szhong/IOTrace/sample/basic.cpp:15
     = [0] main(...) in /home/szhong/IOTrace/sample/basic.cpp:18
    < read(...)
    > close(4) = 0 in 1176ns
     = backtraces:
     = [3] some_namespace::io_fn(char const*) in /home/szhong/IOTrace/sample/basic.cpp:5
     = [2] some_namespace::static_fn(int, int) in /home/szhong/IOTrace/sample/basic.cpp:11
     = [1] some_namespace::inline_fn() in /home/szhong/IOTrace/sample/basic.cpp:15
     = [0] main(...) in /home/szhong/IOTrace/sample/basic.cpp:18
    < close(...)
   < some_namespace::io_fn(char const*) in /home/szhong/IOTrace/sample/basic.cpp:5
  < some_namespace::static_fn(int, int) in /home/szhong/IOTrace/sample/basic.cpp:11
 < some_namespace::inline_fn() in /home/szhong/IOTrace/sample/basic.cpp:15
< main(...) in /home/szhong/IOTrace/sample/basic.cpp:18
```

## Sample Traces

- LevelDB basic: put a single value into the database, get it back, and delete
  it.
    - Source code: [sample/leveldb.cpp](sample/leveldb.cpp)

    - I/O trace: [traces/leveldb-io.txt](traces/leveldb-io.txt)

    - Full trace: [traces/leveldb-full.txt](traces/leveldb-full.txt)

  ```shell
  make trace_leveldb
  TRACE_LOG_FN=0 TRACE_LOG_FILE=traces/leveldb-io.txt ./build/sample/trace_leveldb
  TRACE_LOG_FN=1 TRACE_LOG_FILE=traces/leveldb-full.txt ./build/sample/trace_leveldb
  ```

- LevelDB benchmark
    - Source code:
      [leveldb/benchmarks/db_bench.cc](https://github.com/google/leveldb/blob/main/benchmarks/db_bench.cc)

    - I/O trace: [traces/leveldb_bench.txt](traces/leveldb_bench.txt)

  ```shell
  make db_bench
  TRACE_LOG_FN=0 TRACE_LOG_FILE=traces/leveldb_bench.txt build/_deps/leveldb-build/db_bench --num=100
  ```

