cmake_minimum_required(VERSION 3.10)
project(parallel_data LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

# Список папок с лабораторными работами
set(LAB_DIRS task1 task2 task3 task4)

foreach(LAB_DIR ${LAB_DIRS})
    # Получаем все .cpp файлы в папке
    file(GLOB SRC_FILES "${LAB_DIR}/*.cpp")
    
    foreach(SRC_FILE ${SRC_FILES})
        # Извлекаем имя файла без расширения (например, task2)
        get_filename_component(TARGET_NAME ${SRC_FILE} NAME_WE)
        
        # Формируем уникальное имя цели (например, task1_task2)
        set(FULL_TARGET_NAME "${LAB_DIR}_${TARGET_NAME}")
        
        # Создаем исполняемый файл
        add_executable(${FULL_TARGET_NAME} ${SRC_FILE})
        
        # Подключаем OpenMP
        target_link_libraries(${FULL_TARGET_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endforeach()
endforeach()