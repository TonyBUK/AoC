cmake_minimum_required(VERSION 3.10)

# set the project name
project(solver)

set(CXX_STANDARD 98)

if (MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-O2)
endif()

# add the executable
add_executable(solver solver.cpp cposition.cpp)