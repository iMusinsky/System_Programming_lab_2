cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_STANDARD 11)

option(BUILD_CLIENT "Build client app" ON)
option(BUILD_SERVER "Build server app" ON)

set(RootDir ${CMAKE_CURRENT_SOURCE_DIR})
set(INCLUDE_PATH ${RootDir}/include)
set(SERVER_DIR "server")
set(CLIENT_DIR "client")


SET(ERR_C_FLAGS "-Werror -pedantic-errors -Wall -Wextra -Wpedantic -Wcast-align -Wcast-qual")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ERR_C_FLAGS}")
message("SET FLAGS: ${CMAKE_C_FLAGS}")


if (${BUILD_CLIENT})
    add_subdirectory(${CLIENT_DIR})
endif(${BUILD_CLIENT})

if (${BUILD_SERVER})
    add_subdirectory(${SERVER_DIR})
endif(${BUILD_SERVER})
