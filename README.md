# libcalltrace

A library for tracing function calls and I/O system calls.

## Usage

1. (Optional) Add the following flags to the compiler's command line:

   ```
   -finstrument-functions \
   -finstrument-functions-exclude-file-list=/usr/include \
   -rdynamic
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
      table in the `.dynsym` section. Currently, we use
      [`dladdr(3)`](https://man7.org/linux/man-pages/man3/dladdr.3.html) to
      resolve the function name given a function address. However, `dladdr` can
      only resolve the symbols in the dynamic library.

      If the flag is not set, the library can only print the relative address of
      the function in the object file. One can use `nm` to resolve the function
      name. For example, given the trace output `0x11a0 in trace_basic`, one can
      use the following commands to get the function name:
      ```shell
      nm -C trace_basic | grep 11a0 # prints `00000000000011a0 T main`
      ```

     </details>

2. Run your program with `libcalltrace.so` loaded.

## Example

Compile [sample/basic.cpp](sample/basic.cpp) and run it with `libcalltrace.so`

```shell
g++ sample/basic.cpp -O2 -finstrument-functions -finstrument-functions-exclude-file-list=/usr/include -rdynamic
make # compile libcalltrace.so
LD_PRELOAD=./build/libcalltrace.so ./a.out
```

Output:

```cpp
> main(...)
 > some_namespace::test3(char const*)
  > some_namespace::test2(char const*, int, int)
   > some_namespace::test(char const*)
    > open("/dev/null", 0) = 3 in 6858ns
     = backtraces:
     = [3] some_namespace::test(char const*)
     = [2] some_namespace::test2(char const*, int, int)
     = [1] some_namespace::test3(char const*)
     = [0] main(...)
    < open(...)
    > read(3, 0x7ffe67ef9fb0, 0) = 0 in 380ns
     = backtraces:
     = [3] some_namespace::test(char const*)
     = [2] some_namespace::test2(char const*, int, int)
     = [1] some_namespace::test3(char const*)
     = [0] main(...)
    < read(...)
    > close(3) = 0 in 905ns
     = backtraces:
     = [3] some_namespace::test(char const*)
     = [2] some_namespace::test2(char const*, int, int)
     = [1] some_namespace::test3(char const*)
     = [0] main(...)
    < close(...)
   < some_namespace::test(char const*)
  < some_namespace::test2(char const*, int, int)
 < some_namespace::test3(char const*)
< main(...)
```

## Sample Traces

- LevelDB basic: put a single value into the database, get it back, and delete
  it.
    - Source code: [sample/leveldb.cpp](sample/leveldb.cpp)

    - Full trace: [traces/leveldb-full.txt](traces/leveldb-full.txt)

    - I/O trace: [traces/leveldb-io.txt](traces/leveldb-io.txt)

  ```shell
  make trace_leveldb
  TRACE_LOG_FN=1 TRACE_LOG_FILE=traces/leveldb-full.txt ./build/sample/trace_leveldb
  TRACE_LOG_FN=0 TRACE_LOG_FILE=traces/leveldb-io.txt ./build/sample/trace_leveldb
  ```

- LevelDB benchmark
    - Source code:
      [leveldb/benchmarks/db_bench.cc](https://github.com/google/leveldb/blob/main/benchmarks/db_bench.cc)

    - I/O trace: [traces/leveldb_bench.txt](traces/leveldb_bench.txt)

  ```shell
  make db_bench
  TRACE_LOG_FN=0 TRACE_LOG_FILE=traces/leveldb_bench.txt build/_deps/leveldb-build/db_bench --num=100
  ```

