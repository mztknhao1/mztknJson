cmake_minimum_required(VERSION 3.10)

project(mztknTest)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall -std=c++11 -g -O0")

include_directories(./mztknJson)
aux_source_directory(./mztknJson DIRSRCS)

add_subdirectory(mztknJson)

add_executable(unitTest test.cpp)
target_link_libraries(unitTest mztknJSON)