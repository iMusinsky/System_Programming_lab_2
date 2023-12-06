cmake_minimum_required(VERSION 3.10)

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${CURRENT_DIR}
        LANGUAGES C)

file(GLOB SOURCES
    "*.c"
    "*.h"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_PATH})
