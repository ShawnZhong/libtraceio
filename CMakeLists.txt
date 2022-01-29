cmake_minimum_required(VERSION 3.16)
project(FuncTrace)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")

add_library(trace SHARED trace.cpp trace.h)
target_link_libraries(trace PRIVATE dl)
target_compile_options(trace PRIVATE "-fno-instrument-functions")

# sample_trace
add_executable(sample_trace test/sample.cpp)
target_link_libraries(sample_trace PRIVATE trace)

# test_trace
add_executable(test_trace test/main.cpp test/lib_common.h trace.h)
add_library(test_shared_lib SHARED test/lib_shared.cpp test/lib_common.h)
add_library(test_static_lib STATIC test/lib_static.cpp test/lib_common.h)
target_link_libraries(test_trace trace test_shared_lib test_static_lib)

# leveldb_trace
add_executable(leveldb_trace leveldb.cpp trace.h)
include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)
target_link_libraries(leveldb_trace trace leveldb)
