cmake_minimum_required(VERSION 3.12)
project(COMP3003-Project)
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-Wall -Wextra -pedantic -pthread")

add_executable(client client.c)
add_executable(server server.c)