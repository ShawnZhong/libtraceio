cmake_minimum_required(VERSION 3.16)
project(IOTrace)
set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git)
FetchContent_MakeAvailable(fmt)
set_property(TARGET fmt PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(trace SHARED trace.cpp trace.h)
target_link_libraries(trace PRIVATE dl fmt::fmt)
target_compile_options(trace PRIVATE "-fno-instrument-functions" "-Wl,--no-export-dynamic")
