cmake_minimum_required(VERSION 4.0)
project(parallel_data LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenMP REQUIRED)

include_directories(task1)

add_executable(task2
        task4/task2.cpp
        task4/task2.h
)
target_link_libraries(task2 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task4
        task4/task4.cpp
        task4/task4.h)
target_link_libraries(task4 PRIVATE OpenMP::OpenMP_CXX)

add_executable(task6
        task4/task6.cpp
        task4/task6.h)
target_link_libraries(task6 PRIVATE OpenMP::OpenMP_CXX)