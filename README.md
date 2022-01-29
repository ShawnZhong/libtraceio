# FuncTrace

A simple function and IO tracer for C++.

## Usage

1. Add `-finstrument-functions -Wl,--export-dynamic` to the compiler flags.

2. Run your program with `libtrace.so` loaded.

## Example

Sample program:

```cpp
// test/sample.cpp
#include <fcntl.h>
#include <unistd.h>

void test_io() {
  int fd = open("/dev/null", O_RDONLY);
  read(fd, nullptr, 0);
  close(fd);
}

int main() { test_io(); }
```

Compile and run:

```shell
# compile program
g++ test/sample.cpp -finstrument-functions -Wl,--export-dynamic -o sample

# compile libtrace.so
make

# run program
LD_PRELOAD=./build-release/libtrace.so ./sample
```

Output:

```shell
> main(...)
 > test_io(char const*)
  > open(/dev/null, 0, 0) = 3
   = backtraces: 
   = [2] test_io(char const*)
   = [1] main(...)
  < open
  > read(3, 0, 0) = 0
   = backtraces: 
   = [2] test_io(char const*)
   = [1] main(...)
  < read
  > close(3) = 0
   = backtraces: 
   = [2] test_io(char const*)
   = [1] main(...)
  < close
 < test_io(char const*)
< main(...)
```

## Sample Traces

- [test.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/test.txt)

  ```make && (./build-release/test_trace 2> traces/test.txt)```


- [leveldb.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/leveldb.txt)

  ```make && (./build-release/leveldb_trace 2> traces/leveldb.txt)```


