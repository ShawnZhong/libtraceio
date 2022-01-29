# IOTrace

A simple function and IO tracer

## Usage

1. Add `-finstrument-functions -Wl,--export-dynamic` to the compiler flags.

2. Run your program with `libtrace.so` loaded.

## Example

Sample program: [sample/basic.cpp](sample/basic.cpp)

Compile and run:

```shell
g++ sample/basic.cpp -O2 -finstrument-functions -Wl,--export-dynamic
make # compile libtrace.so
LD_PRELOAD=./build/libtrace.so ./a.out
```

Output:

```cpp
> main(...)
 > test3(char const*)
  > test2(char const*, int, int)
   > test(char const*)
    > open(/dev/null, 0, 0) = 3
     = backtraces: 
     = [4] test(char const*)
     = [3] test2(char const*, int, int)
     = [2] test3(char const*)
     = [1] main(...)
    < open(...)
    > read(3, 0, 0) = 0
     = backtraces: 
     = [4] test(char const*)
     = [3] test2(char const*, int, int)
     = [2] test3(char const*)
     = [1] main(...)
    < read(...)
    > close(3) = 0
     = backtraces: 
     = [4] test(char const*)
     = [3] test2(char const*, int, int)
     = [2] test3(char const*)
     = [1] main(...)
    < close(...)
   < test(char const*)
  < test2(char const*, int, int)
 < test3(char const*)
< main(...)
```

## Sample Traces

- [sample/leveldb.cpp](sample/leveldb.cpp): [traces/leveldb.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/leveldb.txt)

  ```make leveldb_trace && (./build-sample/leveldb_trace 2> traces/leveldb.txt)```


