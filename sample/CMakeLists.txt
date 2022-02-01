cmake_minimum_required(VERSION 3.16)
project(sample)

add_compile_options(-g -finstrument-functions -finstrument-functions-exclude-file-list=/usr/include)
add_link_options(-rdynamic)

# trace_basic
add_executable(trace_basic basic.cpp)

# trace_test
add_executable(trace_test test/main.cpp test/common.h)
add_library(test_shared_lib SHARED test/shared.cpp test/common.h)
add_library(test_static_lib STATIC test/static.cpp test/common.h)
target_link_libraries(trace_test test_shared_lib test_static_lib)

# trace_leveldb
add_executable(trace_leveldb leveldb.cpp)
include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)
target_link_libraries(trace_leveldb leveldb)
