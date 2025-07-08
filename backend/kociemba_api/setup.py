from setuptools import setup, Extension
import os
import pybind11

ext_modules = [
    Extension(
        "kociemba_solver",
        ["src/kociemba_wrapper.cpp", 
         "src/solver/cube.cpp",
         "src/solver/solver.cpp"],
        include_dirs=[pybind11.get_include(), "src"],
        language='c++',
        extra_compile_args=['-std=c++11'],
    ),
]

setup(
    name="kociemba_solver",
    ext_modules=ext_modules,
    install_requires=["pybind11>=2.6.0"],
    python_requires=">=3.6",
)