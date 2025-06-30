cmake_minimum_required(VERSION 3.12)
project(kociemba_solver)

set(CMAKE_CXX_STANDARD 14)

# Find pybind11
find_package(pybind11 REQUIRED)
include_directories(/usr/include/python3.10)
# Add the solver library
add_library(kociemba_lib STATIC
    kociemba_api/src/kociemba_solver.cpp
    kociemba_api/src/kociemba_solver.h
)
set_property(TARGET kociemba_lib PROPERTY POSITION_INDEPENDENT_CODE ON)
# Create Python module
pybind11_add_module(kociemba_solver
    kociemba_api/src/kociemba_wrapper.cpp
)# Link the solver library
target_link_libraries(kociemba_solver PRIVATE kociemba_lib)

# Set properties
target_compile_definitions(kociemba_solver PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})

