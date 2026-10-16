cmake_minimum_required(VERSION 3.10)
project(Lab1 C)
set(CMAKE_C_STANDARD 99)

file(GLOB TASK_DIRS "Task*")

foreach(TASK_DIR ${TASK_DIRS})
    if(IS_DIRECTORY ${TASK_DIR})
        get_filename_component(TASK_NAME ${TASK_DIR} NAME)
        file(GLOB SOURCE_FILES "${TASK_DIR}/*.c")
        add_executable(${TASK_NAME} ${SOURCE_FILES})
        # Подключаем математическую библиотеку
        target_link_libraries(${TASK_NAME} m)
    endif()
endforeach()