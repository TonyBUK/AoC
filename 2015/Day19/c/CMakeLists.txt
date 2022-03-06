cmake_minimum_required(VERSION 3.10)

# set the project name
project(solver)

set(C_STANDARD 90)
include_directories(hashmap/include/)

# add the executable
add_executable(solver solver.c hashmap/src/hashmap.c)