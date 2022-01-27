cmake_minimum_required(VERSION 3.16)
project(FuncTrace)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)

add_executable(test_trace test.cpp trace.h)
target_link_libraries(test_trace dl)
target_compile_options(test_trace PRIVATE -finstrument-functions)

add_executable(leveldb_trace leveldb.cpp trace.h)
target_link_libraries(leveldb_trace dl leveldb)
target_compile_options(leveldb_trace PRIVATE -finstrument-functions)
