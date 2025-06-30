#include "kociemba_solver.h"
#include <iostream>
#include <cassert>
#include <chrono>

int main() {
    std::cout << "Testing Kociemba Two-Phase Algorithm Solver..." << std::endl;
    
    KociembaSolver solver;
    
    // Test 1: Generate scramble
    std::cout << "\nTest 1: Generate scramble" << std::endl;
    std::string scramble = solver.generateScramble(15);
    std::cout << "Generated scramble: " << scramble << std::endl;
    
    // Test 2: Convert scramble to cube string
    std::cout << "\nTest 2: Convert scramble to cube string" << std::endl;
    std::string cubeString = solver.scrambleToCubeString(scramble);
    std::cout << "Cube string: " << cubeString << std::endl;
    
    // Test 3: Validate cube string
    std::cout << "\nTest 3: Validate cube string" << std::endl;
    bool isValid = solver.isValidCube(cubeString);
    std::cout << "Cube string is valid: " << (isValid ? "Yes" : "No") << std::endl;
    
    // Test 4: Solve cube (simplified test with solved cube)
    std::cout << "\nTest 4: Solve cube" << std::endl;
    std::string solvedCube = "000000000111111111222222222333333333444444444555555555";
    std::string solution = solver.solve(solvedCube);
    std::cout << "Solution for solved cube: '" << solution << "'" << std::endl;
    
    // Test 5: Test C interface
    std::cout << "\nTest 5: Test C interface" << std::endl;
    const char* cScramble = kociemba_generate_scramble();
    std::cout << "C interface - Generated scramble: " << cScramble << std::endl;
    
    const char* cCubeString = scramble_to_cube_string(cScramble);
    std::cout << "C interface - Cube string: " << cCubeString << std::endl;
    
    int cIsValid = kociemba_is_valid_cube(cCubeString);
    std::cout << "C interface - Is valid: " << (cIsValid ? "Yes" : "No") << std::endl;
    
    const char* cSolution = kociemba_solve(cCubeString);
    std::cout << "C interface - Solution: " << cSolution << std::endl;
    
    // Test 6: Performance test
    std::cout << "\nTest 6: Performance test" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; i++) {
        std::string testScramble = solver.generateScramble(10);
        std::string testCube = solver.scrambleToCubeString(testScramble);
        std::string testSolution = solver.solve(testCube);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "10 solve operations took: " << duration.count() << " ms" << std::endl;
    
    std::cout << "\nKociemba solver tests completed!" << std::endl;
    return 0;
}

