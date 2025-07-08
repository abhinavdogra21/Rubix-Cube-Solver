#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "solver/solver.h"

namespace py = pybind11;

namespace {

std::string solve_cube(const std::string& cube_state) {
    try {
        // Create solver instance
        kociemba::Solver solver;
        
        // Get solution moves
        std::vector<kociemba::Move> solution = solver.solve(cube_state);
        
        // Convert solution to string
        std::string result;
        for (size_t i = 0; i < solution.size(); i++) {
            if (i > 0) result += " ";
            result += kociemba::Cube::moveToString(solution[i]);
        }
        
        return result;
    } catch (const std::exception& e) {
        throw py::value_error(e.what());
    }
}

} // anonymous namespace

PYBIND11_MODULE(kociemba_solver, m) {
    m.doc() = "Kociemba two-phase Rubik's cube solver"; 
    m.def("solve", &solve_cube, "Solve a Rubik's cube using Kociemba's algorithm",
          py::arg("cube_state"));
}


