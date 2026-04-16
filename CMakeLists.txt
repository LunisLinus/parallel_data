# для lab1-6
#cmake_minimum_required(VERSION 3.10)
#project(parallel_data LANGUAGES CXX)
#
#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#
#find_package(OpenMP REQUIRED)
#find_package(BLAS REQUIRED)
#find_package(LAPACK REQUIRED)
#
#set(LAB_DIRS lab1 lab2 lab3 lab4 lab5 lab7)
#
#foreach(LAB_DIR ${LAB_DIRS})
#    file(GLOB SRC_FILES "${LAB_DIR}/*.cpp")
#
#    foreach(SRC_FILE ${SRC_FILES})
#        get_filename_component(TARGET_NAME ${SRC_FILE} NAME_WE)
#        set(FULL_TARGET_NAME "${LAB_DIR}_${TARGET_NAME}")
#
#        add_executable(${FULL_TARGET_NAME} ${SRC_FILE})
#        target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
#    endforeach()
#endforeach()
#
#file(GLOB LAB6_SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lab6/*")
#
#foreach(SUBDIR ${LAB6_SUBDIRS})
#    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}")
#        file(GLOB SRC_FILES "${SUBDIR}/*.cpp")
#        get_filename_component(SUBDIR_NAME ${SUBDIR} NAME)
#
#        foreach(SRC_FILE ${SRC_FILES})
#            get_filename_component(FILE_NAME ${SRC_FILE} NAME_WE)
#            set(FULL_TARGET_NAME "lab6_${FILE_NAME}")
#
#            add_executable(${FULL_TARGET_NAME} ${SRC_FILE}
#                    lab7/task2.cpp)
#
#            if(SUBDIR_NAME STREQUAL "Matr_Mult")
#                target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
#            elseif(SUBDIR_NAME STREQUAL "Matr_Mult_BLAS")
#                target_link_libraries(${FULL_TARGET_NAME} PRIVATE BLAS::BLAS)
#            elseif(SUBDIR_NAME STREQUAL "Lin_Solv_Lapack")
#                target_link_libraries(${FULL_TARGET_NAME} PRIVATE LAPACK::LAPACK BLAS::BLAS)
#            else()
#                target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
#            endif()
#        endforeach()
#    endif()
#endforeach()

# для lab7-9
cmake_minimum_required(VERSION 3.10)
project(parallel_data LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(lab7_task2 lab7/task2.cpp)
add_executable(lab8_task4 lab8/task4.cpp)
add_executable(lab9_task6 lab9/task6.cpp) # /opt/homebrew/bin/mpirun -n 4 ./cmake-build-debug/lab9_task6