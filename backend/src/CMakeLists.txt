cmake_minimum_required(VERSION 3.12)
project(CustomCubeSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Python
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# Include directories
include_directories(${Python3_INCLUDE_DIRS})
include_directories(.)

# Source files
set(SOURCES
    custom_cube_solver.cpp
)

# Create shared library for Python binding
add_library(custom_cube_solver SHARED ${SOURCES})
target_link_libraries(custom_cube_solver ${Python3_LIBRARIES})

# Set library properties
set_target_properties(custom_cube_solver PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

# Create standalone executable for testing
add_executable(test_solver test_solver.cpp ${SOURCES})

# Compiler flags
target_compile_options(custom_cube_solver PRIVATE -fPIC -O3)
target_compile_options(test_solver PRIVATE -O3)

