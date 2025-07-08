cmake_minimum_required(VERSION 3.4...3.18)
# Set CMake policy for modern Python finder
cmake_policy(SET CMP0148 NEW)
set(PYBIND11_FINDPYTHON ON)

project(kociemba_solver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Explicitly find Python3 interpreter and development headers
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Disable strip to avoid shell errors with spaces/parentheses in path
set(CMAKE_STRIP "")

# find_package instead of add_subdirectory for pybind11
find_package(pybind11 REQUIRED)

pybind11_add_module(kociemba_solver 
    src/kociemba_wrapper.cpp
    src/solver/solver.cpp
    src/solver/cube.cpp
)

target_include_directories(kociemba_solver PRIVATE src)