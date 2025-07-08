cmake_minimum_required(VERSION 3.4...3.18)
project(kociemba_solver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(pybind11)

pybind11_add_module(kociemba_solver 
    src/kociemba_wrapper.cpp
    src/solver/solver.cpp
    src/solver/cube.cpp
)

target_include_directories(kociemba_solver PRIVATE src) 