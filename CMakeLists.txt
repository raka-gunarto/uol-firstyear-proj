# change this if you happen to be working with a lower version of CMake
# Surprise! DEC-10 machines have v2.8
cmake_minimum_required(VERSION 2.8)

project(
    "coursework_one"
    C
)

# change this to Release to activate optimization and compile without debug symbols
set(CMAKE_BUILD_TYPE Debug)

# enable/disable externs depending on build type
if(CMAKE_BUILD_TYPE MATCHES Debug)
	message("[WARN] enabling testing flag for exposing internal module variables")
	add_definitions(-DTESTING_FLAG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# adds arbitrary flags to the compiler.
# this one is used to generate the files we give in input to Egypt.
set(CMAKE_C_FLAGS "-std=c99 -fdump-rtl-expand")

# CMake test integration
include(CTest)

# add subdirectories
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(external)
