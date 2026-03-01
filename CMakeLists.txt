cmake_minimum_required(VERSION 4.0)
project(parallel_data LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenMP REQUIRED)

include_directories(task1)

add_executable(parallel_data
        task2/task4.cpp
        task2/task4.h
        task2/task4.cpp
        task2/task4.h)

target_link_libraries(parallel_data PRIVATE OpenMP::OpenMP_CXX)