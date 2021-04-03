cmake_minimum_required(VERSION 3.10)

project(mztknTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -std=c++11 -O0")

include_directories(./mztknJson)
aux_source_directory(./mztknJson DIRSRCS)

# add_subdirectory(mztknJson)

add_executable(unitTest test.cpp ./mztknJson/value.h ./mztknJson/parser.h ./mztknJson/value.cpp ./mztknJson/parser.cpp)