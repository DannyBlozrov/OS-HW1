cmake_minimum_required(VERSION 3.23)
project(HW1 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread")
add_executable(HW1 main.c)
