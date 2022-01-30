# libcalltrace

A library for tracing function calls and I/O syscalls.

## Usage

1. Add `-finstrument-functions -Wl,--export-dynamic` to the compiler flags.

2. Run your program with `libcalltrace.so` loaded.

## Example

Sample program: [sample/basic.cpp](sample/basic.cpp)

Compile and run:

```shell
g++ sample/basic.cpp -O2 -finstrument-functions -Wl,--export-dynamic
make # compile libcalltrace.so
LD_PRELOAD=./build/libcalltrace.so ./a.out
```

Output:

```cpp
> main(...)
 > test3(char const*)
  > test2(char const*, int, int)
   > test(char const*)
    > open(/dev/null, 0) = 3 in 6232 ns
     = backtraces:
     = [3] test(char const*)
     = [2] test2(char const*, int, int)
     = [1] test3(char const*)
     = [0] main(...)
    < open(...)
    > read(3, 0, 0) = 0 in 362 ns
     = backtraces:
     = [3] test(char const*)
     = [2] test2(char const*, int, int)
     = [1] test3(char const*)
     = [0] main(...)
    < read(...)
    > close(3) = 0 in 1104 ns
     = backtraces:
     = [3] test(char const*)
     = [2] test2(char const*, int, int)
     = [1] test3(char const*)
     = [0] main(...)
    < close(...)
   < test(char const*)
  < test2(char const*, int, int)
 < test3(char const*)
< main(...)
```

## Sample Traces

- [sample/leveldb.cpp](sample/leveldb.cpp): [traces/leveldb-full.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/leveldb-full.txt)
  , [traces/leveldb-io.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/leveldb-io.txt)

  ```
  make leveldb_trace
  TRACE_PRINT_FN_TRACE=1 ./build-sample/leveldb_trace 2> traces/leveldb-full.txt
  TRACE_PRINT_FN_TRACE=0 ./build-sample/leveldb_trace 2> traces/leveldb-io.txt
  ```


