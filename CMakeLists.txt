cmake_minimum_required(VERSION 3.16)
project(FuncTrace)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -g -Wl,--export-dynamic -fno-omit-frame-pointer")
link_libraries(dl)

include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)

add_executable(test_trace test.cpp trace.h)
add_executable(leveldb_trace leveldb.cpp trace.h)
target_link_libraries(leveldb_trace leveldb)
