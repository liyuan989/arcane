cmake_minimum_required(VERSION 2.8.11)

project(arcane)

set(CMAKE_VERBOSE_MAKEFILE ON)

if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "Compiler support GNU gcc only")
endif()
if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8.5)
    message(FATAL_ERROR "GCC>=4.8.5 required")
endif()

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

exec_program("git" ${CMAKE_SOURCE_DIR} ARGS "log --format='[sha1]:%H [author]:%cn [time]:%ci [commit]:\"%s\" [branch]:%d' -1" OUTPUT_VARIABLE VERSION_SHA1)
message(STATUS "git commit version: ${VERSION_SHA1}")

set(CMAKE_CXX_FLAGS
    -g
    -ggdb
    -Wall
    -Wextra
    -Werror
    -Wshadow
    -Wold-style-cast
    -Woverloaded-virtual
    -Wpointer-arith
    -Wreorder
    -Wuninitialized
    -Wunreachable-code
    -pedantic
    -rdynamic
    -pthread
    -D_FILE_OFFSET_BITS=64
    -std=c++1y
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # -m64 is the default for a 64-bit compiler
    message(STATUS "Building on a 64 bit system")
else()
    list(APPEND CMAKE_CXX_FLAGS "-m32")
    message(STATUS "Building on a 32 bit system")
endif()

string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type specified, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/Debug)
    message(STATUS "Configuring routing-adaptor in debug mode")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib/Release)
    message(STATUS "Configuring routing-adaptor in release mode")
else()
    message(FATAL_ERROR "Wrong build option! usage: -DCMAKE_BUILD_TYPE=Debug/Release")
endif()

include_directories(${CMAKE_SOURCE_DIR})

file(GLOB_RECURSE SRCS ${CMAKE_SOURCE_DIR}/arcane/*.cpp)

add_library(arcane ${SRCS})

set(LIBRARIES
    -Wl,--start-group
    pthread
    rt
    -Wl,--end-group
)

target_link_libraries(arcane ${LIBRARIES})

