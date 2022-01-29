# FuncTrace

A simple function tracer for C++.

## Usage

1. Add `#include "trace.h"` to your source file.
2. Add `-finstrument-functions -Wl,--export-dynamic` to the compiler flags.
3. Add `-ldl` to the linker flags.
4. Run the program.

## Example Traces

```make && (./build-release/leveldb_trace 2> traces/leveldb.txt)```

```make && (./build-release/test_trace 2> traces/test.txt)```
