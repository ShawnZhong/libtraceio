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

# trace_sqlite
FetchContent_Declare(sqlite3 URL "https://www.sqlite.org/2022/sqlite-amalgamation-3370200.zip")
FetchContent_MakeAvailable(sqlite3)
add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c)
target_link_libraries(sqlite3 dl pthread)
add_executable(trace_sqlite sqlite.cpp)
target_include_directories(trace_sqlite PUBLIC ${sqlite3_SOURCE_DIR})
target_link_libraries(trace_sqlite sqlite3)

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
