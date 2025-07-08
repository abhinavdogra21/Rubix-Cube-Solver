#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "solver/solver.h"

namespace py = pybind11;

// Expose the function directly in the global namespace for pybind11
// This ensures the symbol is correctly found by ctypes
extern "C" std::string kociemba_solve(const std::string& cube_state) {
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
        // Re-throw as a Python exception
        throw py::value_error(e.what());
    }
}

// Other C++ functions that need to be exposed via ctypes should also be extern "C"
extern "C" std::string kociemba_generate_scramble() {
    // Implement your scramble generation logic here
    // For now, return a placeholder or throw an error if not implemented
    return "R U R' U'"; // Placeholder
}

extern "C" int kociemba_is_valid_cube(const std::string& cube_state) {
    // Implement your cube validation logic here
    return 1; // Placeholder for valid
}

extern "C" std::string scramble_to_cube_string(const std::string& scramble) {
    // Implement your scramble to cube string conversion logic here
    return "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB"; // Placeholder
}

PYBIND11_MODULE(kociemba_solver, m) {
    m.doc() = "Kociemba two-phase Rubik's cube solver"; 
    // No need to define m.def("kociemba_solve", ...) here if using ctypes directly
    // The ctypes wrapper will find the extern "C" functions
}


