#include "custom_cube_solver.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Custom Cube Solver..." << std::endl;
    
    CustomCubeSolver solver;
    
    // Test 1: Generate scramble
    std::cout << "\nTest 1: Generate scramble" << std::endl;
    std::string scramble = solver.generateScramble(10);
    std::cout << "Generated scramble: " << scramble << std::endl;
    
    // Test 2: Validate scramble
    std::cout << "\nTest 2: Validate scramble" << std::endl;
    bool isValid = solver.isValidScramble(scramble);
    std::cout << "Scramble is valid: " << (isValid ? "Yes" : "No") << std::endl;
    assert(isValid);
    
    // Test 3: Solve scramble
    std::cout << "\nTest 3: Solve scramble" << std::endl;
    std::string solution = solver.solve(scramble);
    std::cout << "Solution: " << solution << std::endl;
    
    // Test 4: Test invalid scramble
    std::cout << "\nTest 4: Test invalid scramble" << std::endl;
    bool invalidTest = solver.isValidScramble("X Y Z");
    std::cout << "Invalid scramble test: " << (invalidTest ? "Failed" : "Passed") << std::endl;
    assert(!invalidTest);
    
    // Test 5: Test empty scramble
    std::cout << "\nTest 5: Test empty scramble" << std::endl;
    std::string emptySolution = solver.solve("");
    std::cout << "Empty scramble solution: '" << emptySolution << "'" << std::endl;
    
    // Test 6: Test C interface
    std::cout << "\nTest 6: Test C interface" << std::endl;
    const char* cScramble = "R U R' U'";
    const char* cSolution = solve_cube(cScramble);
    std::cout << "C interface - Scramble: " << cScramble << std::endl;
    std::cout << "C interface - Solution: " << cSolution << std::endl;
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}

