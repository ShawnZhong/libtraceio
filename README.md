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

void test(const char* pathname) {
  int fd = open(pathname, O_RDONLY);
  int rc = read(fd, nullptr, 0);
  close(fd);
}
int test2(const char* pathname, int a, int b) {
  test(pathname);
  return a + b;
}
int test3(const char* pathname) { return test2(pathname, 1, -1); }

int main() { return test3("/dev/null"); }

```

Compile and run:

```shell
# compile program
g++ test/sample.cpp -O2 -finstrument-functions -Wl,--export-dynamic -o sample

# compile libtrace.so
make

# run program
LD_PRELOAD=./build/libtrace.so ./sample
```

Output:

```shell
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
    < open
    > read(3, 0, 0) = 0
     = backtraces: 
     = [4] test(char const*)
     = [3] test2(char const*, int, int)
     = [2] test3(char const*)
     = [1] main(...)
    < read
    > close(3) = 0
     = backtraces: 
     = [4] test(char const*)
     = [3] test2(char const*, int, int)
     = [2] test3(char const*)
     = [1] main(...)
    < close
   < test(char const*)
  < test2(char const*, int, int)
 < test3(char const*)
< main(...)
```

## Sample Traces

- [test.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/test.txt)

  ```make && (./build/test_trace 2> traces/test.txt)```


- [leveldb.txt](https://raw.githubusercontent.com/ShawnZhong/FuncTrace/main/traces/leveldb.txt)

  ```make && (./build/leveldb_trace 2> traces/leveldb.txt)```


