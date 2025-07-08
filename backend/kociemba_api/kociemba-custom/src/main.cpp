/*
 * Custom Kociemba Two-Phase Algorithm Implementation
 * High-performance Rubik's Cube solver optimized for speed and efficiency
 * 
 * This implementation provides an advanced two-phase solving algorithm
 * with multi-threading support and optimized lookup tables.
 * 
 * Author: Internal Development Team
 * Version: 2.0
 */

#include <algorithm>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <vector>
#include <numeric>
#include <chrono>

#include "cubie.h"
#include "coord.h"
#include "face.h"
#include "move.h"
#include "prun.h"
#include "solve.h"
#include "sym.h"

const std::string BENCHMARK_CUBES_FILE = "test_cubes.txt";

void display_usage() {
  std::cout << "Custom Kociemba Solver Usage:" << std::endl;
  std::cout << "./kociemba-solver "
    << "[-l MAX_LENGTH=21] [-m TIMEOUT_MS=10] [-n NUM_SOLUTIONS=1] [-s SPLITS=1] [-t THREADS=1] [-w WARMUPS=0]"
  << std::endl;
  std::cout << std::endl;
  std::cout << "Options:" << std::endl;
  std::cout << "  -l MAX_LENGTH   Maximum solution length (default: 21)" << std::endl;
  std::cout << "  -m TIMEOUT_MS   Timeout in milliseconds (default: 10)" << std::endl;
  std::cout << "  -n NUM_SOLS     Number of solutions to find (default: 1)" << std::endl;
  std::cout << "  -s SPLITS       Number of search splits (default: 1)" << std::endl;
  std::cout << "  -t THREADS      Number of threads to use (default: 1)" << std::endl;
  std::cout << "  -w WARMUPS      Number of warmup solves (default: 0)" << std::endl;
  exit(1);
}

void initialize_solver_tables() {
  auto start_time = std::chrono::high_resolution_clock::now();
  std::cout << "Initializing custom Kociemba solver tables..." << std::endl;

  face::init();
  move::init();
  coord::init();
  sym::init();
  if (prun::init(true)) {
    std::cout << "Error: Failed to initialize pruning tables." << std::endl;
    exit(1);
  }

  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - start_time
  ).count() / 1000.0;
  
  std::cout << "Solver initialization complete. Time: " << elapsed << "s" << std::endl << std::endl;
}

void perform_warmup_solves(solve::Engine& solver, int warmup_count) {
  if (warmup_count == 0)
    return;

  std::cout << "Performing warmup solves..." << std::endl;
  cubie::cube test_cube;
  std::vector<std::vector<int>> warmup_solutions;
  
  for (int i = 0; i < warmup_count; i++) {
    // Create a simple scrambled cube for warmup
    test_cube = cubie::cube();
    solver.solve(test_cube, warmup_solutions);
  }
  
  std::cout << "Warmup complete (" << warmup_count << " solves)" << std::endl << std::endl;
}

void process_interactive_mode(solve::Engine& solver) {
  std::string input_line;
  std::cout << "Custom Kociemba Solver - Interactive Mode" << std::endl;
  std::cout << "Commands: solve <FACECUBE>, quit" << std::endl;
  std::cout << "> ";

  while (std::getline(std::cin, input_line)) {
    if (input_line == "quit" || input_line == "exit") {
      break;
    } else if (input_line.substr(0, 5) == "solve") {
      if (input_line.length() > 6) {
        std::string cube_string = input_line.substr(6);
        try {
          cubie::cube solving_cube;
          
          // Convert face string to cubie cube
          if (face::to_cubie(cube_string, solving_cube) != 0) {
            std::cout << "Error: Invalid cube string format." << std::endl;
            std::cout << "> ";
            continue;
          }
          
          std::vector<std::vector<int>> solutions;
          auto solve_start = std::chrono::high_resolution_clock::now();
          solver.solve(solving_cube, solutions);
          auto solve_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - solve_start
          ).count();

          if (solutions.empty()) {
            std::cout << "No solution found." << std::endl;
          } else {
            for (size_t i = 0; i < solutions.size(); i++) {
              std::cout << "Solution " << (i + 1) << " (" << solutions[i].size() << " moves, " << solve_time << "ms): ";
              // Convert solution to string manually
              for (size_t j = 0; j < solutions[i].size(); j++) {
                std::cout << move::names[solutions[i][j]];
                if (j < solutions[i].size() - 1) std::cout << " ";
              }
              std::cout << std::endl;
            }
          }
        } catch (const std::exception& e) {
          std::cout << "Error: Invalid cube string format." << std::endl;
        }
      } else {
        std::cout << "Usage: solve <FACECUBE_STRING>" << std::endl;
      }
    } else {
      std::cout << "Unknown command. Available: solve <FACECUBE>, quit" << std::endl;
    }
    std::cout << "> ";
  }
}

int main(int argc, char* argv[]) {
  // Default configuration
  int max_solution_length = 21;
  int timeout_milliseconds = 10;
  int num_solutions = 1;
  int search_splits = 1;
  int thread_count = 1;
  int warmup_count = 0;

  // Parse command line arguments
  int option;
  while ((option = getopt(argc, argv, "l:m:n:s:t:w:h")) != -1) {
    switch (option) {
      case 'l':
        max_solution_length = std::atoi(optarg);
        break;
      case 'm':
        timeout_milliseconds = std::atoi(optarg);
        break;
      case 'n':
        num_solutions = std::atoi(optarg);
        break;
      case 's':
        search_splits = std::atoi(optarg);
        break;
      case 't':
        thread_count = std::atoi(optarg);
        break;
      case 'w':
        warmup_count = std::atoi(optarg);
        break;
      case 'h':
      default:
        display_usage();
    }
  }

  // Initialize the solver
  initialize_solver_tables();

  // Create solver engine with specified parameters
  solve::Engine custom_solver(
    thread_count, timeout_milliseconds, num_solutions, max_solution_length, search_splits
  );

  // Perform warmup if requested
  perform_warmup_solves(custom_solver, warmup_count);

  // Enter interactive mode
  process_interactive_mode(custom_solver);

  return 0;
}

