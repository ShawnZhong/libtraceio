cmake_minimum_required(VERSION 3.16)
project(FuncTrace)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")
link_libraries(dl)

include_directories(${CMAKE_SOURCE_DIR})

# test_trace
add_executable(test_trace test/main.cpp test/common.h trace.h)
add_library(test_shared_lib SHARED test/shared_lib.cpp test/common.h)
add_library(test_static_lib STATIC test/static_lib.cpp test/common.h)
target_link_libraries(test_trace test_shared_lib test_static_lib)

# leveldb_trace
add_executable(leveldb_trace leveldb.cpp trace.h)
include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)
target_link_libraries(leveldb_trace leveldb)
