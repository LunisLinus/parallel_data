cmake_minimum_required(VERSION 3.10)
project(parallel_data LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

# lab1-lab5
set(LAB_DIRS lab1 lab2 lab3 lab4 lab5)

foreach(LAB_DIR ${LAB_DIRS})
    file(GLOB SRC_FILES "${LAB_DIR}/*.cpp")

    foreach(SRC_FILE ${SRC_FILES})
        get_filename_component(TARGET_NAME ${SRC_FILE} NAME_WE)
        set(FULL_TARGET_NAME "${LAB_DIR}_${TARGET_NAME}")

        add_executable(${FULL_TARGET_NAME} ${SRC_FILE})
        target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endforeach()
endforeach()

# lab6/task1
file(GLOB LAB6_TASK1_SUBDIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "lab6/task1/*")

foreach(SUBDIR ${LAB6_TASK1_SUBDIRS})
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}")
        file(GLOB SRC_FILES "${SUBDIR}/*.cpp")
        get_filename_component(SUBDIR_NAME ${SUBDIR} NAME)

        foreach(SRC_FILE ${SRC_FILES})
            set(FULL_TARGET_NAME "lab6_task1_${SUBDIR_NAME}")

            add_executable(${FULL_TARGET_NAME} ${SRC_FILE})

            if(SUBDIR_NAME STREQUAL "Matr_Mult")
                target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
            elseif(SUBDIR_NAME STREQUAL "Matr_Mult_BLAS")
                target_link_libraries(${FULL_TARGET_NAME} PRIVATE BLAS::BLAS)
            elseif(SUBDIR_NAME STREQUAL "Lin_Solv_Lapack")
                target_link_libraries(${FULL_TARGET_NAME} PRIVATE LAPACK::LAPACK BLAS::BLAS)
            else()
                target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
            endif()
        endforeach()
    endif()
endforeach()