#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "kociemba_solver.h"

namespace py = pybind11;

PYBIND11_MODULE(kociemba_solver, m) {
    m.doc() = "pybind11 example plugin"; // optional module docstring

    py::class_<KociembaSolver>(m, "KociembaSolver")
        .def(py::init<>())
        .def("solve", &KociembaSolver::solve)
        .def("generateScramble", &KociembaSolver::generateScramble)
        .def("isValidCube", &KociembaSolver::isValidCube)
        .def("scrambleToCubeString", &KociembaSolver::scrambleToCubeString);

    m.def("kociemba_solve", &KociembaSolver::solve, "A function that solves a cube");
    m.def("kociemba_generate_scramble", &KociembaSolver::generateScramble, "A function that generates a scramble");
    m.def("kociemba_is_valid_cube", &KociembaSolver::isValidCube, "A function that validates a cube string");
    m.def("scramble_to_cube_string", &KociembaSolver::scrambleToCubeString, "A function that converts scramble to cube string");
}


