cmake_minimum_required(VERSION 3.16)
project(sample)

add_subdirectory(.. ../build)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finstrument-functions -Wl,--export-dynamic")

# basic_trace
add_executable(basic_trace basic.cpp)
target_link_libraries(basic_trace PRIVATE trace)

# test_trace
add_executable(test_trace test.cpp test_common.h)
add_library(test_shared_lib SHARED test_shared.cpp test_common.h)
add_library(test_static_lib STATIC test_static.cpp test_common.h)
target_link_libraries(test_trace PRIVATE trace test_shared_lib test_static_lib)

# leveldb_trace
add_executable(leveldb_trace leveldb.cpp)
include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)
target_link_libraries(leveldb_trace PRIVATE trace leveldb)
