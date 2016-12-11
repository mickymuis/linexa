cmake_minimum_required (VERSION 2.8)

project (linexa)

#set( CMAKE_VERBOSE_MAKEFILE ON )

list (APPEND CMAKE_C_FLAGS "-g -std=c99")

    add_executable(linexa 
        src/main.c 
        src/linexa.c )

target_link_libraries (linexa "-lm" )
