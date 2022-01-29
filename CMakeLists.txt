cmake_minimum_required(VERSION 3.16)
project(IOTrace)
set(CMAKE_CXX_STANDARD 20)

add_library(trace SHARED trace.cpp trace.h)
target_link_libraries(trace PRIVATE dl)
target_compile_options(trace PRIVATE "-fno-instrument-functions" "-Wl,--no-export-dynamic")
