#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "solver/solver.h"

namespace py = pybind11;

// Define the C++ function that will be exposed
std::string solve_cube_cpp(const std::string& cube_state) {
    try {
        kociemba::Solver solver;
        std::vector<kociemba::Move> solution = solver.solve(cube_state);
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

// Expose the C++ function with extern "C" for ctypes compatibility
extern "C" {
    std::string kociemba_solve(const std::string& cube_state) {
        return solve_cube_cpp(cube_state);
    }
    // Placeholder for other functions if needed by ctypes
    std::string kociemba_generate_scramble() {
        return "R U R' U'"; // Placeholder
    }
    int kociemba_is_valid_cube(const std::string& cube_state) {
        return 1; // Placeholder
    }
    std::string scramble_to_cube_string(const std::string& scramble) {
        return "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"; // Placeholder
    }
}

PYBIND11_MODULE(kociemba_solver, m) {
    m.doc() = "Kociemba two-phase Rubik's cube solver"; 
    // Expose the function for direct pybind11 usage
    m.def("solve", &solve_cube_cpp, "Solve a Rubik's cube using Kociemba's algorithm",
          py::arg("cube_state"));
}


