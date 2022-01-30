cmake_minimum_required(VERSION 3.16)
project(sample)

add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/trace)

add_compile_options(-finstrument-functions -finstrument-functions-exclude-file-list=/usr/include)
add_link_options(-Wl,-export-dynamic)

# trace_basic
add_executable(trace_basic basic.cpp)
target_link_libraries(trace_basic PRIVATE calltrace)

# trace_test
add_executable(test_trace test/main.cpp test/common.h)
add_library(test_shared_lib SHARED test/shared.cpp test/common.h)
add_library(test_static_lib STATIC test/static.cpp test/common.h)
target_link_libraries(test_trace PRIVATE calltrace test_shared_lib test_static_lib)

# trace_leveldb
add_executable(trace_leveldb leveldb.cpp)
include(FetchContent)
FetchContent_Declare(
    leveldb
    GIT_REPOSITORY https://github.com/google/leveldb.git
    GIT_TAG main
)
FetchContent_MakeAvailable(leveldb)
target_link_libraries(trace_leveldb PRIVATE calltrace leveldb)

# db_bench
target_link_libraries(db_bench calltrace)
