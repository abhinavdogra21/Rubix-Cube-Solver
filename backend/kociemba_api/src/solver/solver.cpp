#include "solver.h"
#include <chrono>
#include <algorithm>
#include <stdexcept>
#include <queue>
#include <limits>

namespace kociemba {

// Initialize static members
std::vector<std::vector<int>> Solver::twistMove;
std::vector<std::vector<int>> Solver::flipMove;
std::vector<std::vector<int>> Solver::sliceMove;
std::vector<std::vector<int>> Solver::URFtoDLFMove;
std::vector<std::vector<int>> Solver::URtoBRMove;
std::vector<std::vector<int>> Solver::parityMove;

std::vector<int> Solver::twistPrun;
std::vector<int> Solver::flipPrun;
std::vector<int> Solver::slicePrun;
std::vector<int> Solver::parityPrun;
std::vector<int> Solver::URFtoDLFPrun;
std::vector<int> Solver::URtoBRPrun;

Solver::Solver() : maxDepthPhase1(20), maxDepthPhase2(18), timeoutMillis(5000), initialized(false) {
    initTables();
}

std::vector<Move> Solver::solve(const std::string& state) {
    if (!initialized) {
        throw std::runtime_error("Solver not initialized");
    }

    // Create cube from state
    Cube cube(state);

    // Start timer
    auto startTime = std::chrono::steady_clock::now();

    // Phase 1: Orient edges and corners
    std::vector<Move> phase1Solution;
    try {
        phase1Solution = phase1(cube);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Phase 1 failed: " + std::string(e.what()));
    }
    
    // Apply phase 1 solution
    Cube phase1Cube = cube;
    for (Move move : phase1Solution) {
        phase1Cube.applyMove(move);
    }

    // Phase 2: Solve the rest
    std::vector<Move> phase2Solution;
    try {
        phase2Solution = phase2(phase1Cube, phase1Solution);
    } catch (const std::runtime_error& e) {
        throw std::runtime_error("Phase 2 failed: " + std::string(e.what()));
    }

    // Combine solutions
    std::vector<Move> solution = phase1Solution;
    solution.insert(solution.end(), phase2Solution.begin(), phase2Solution.end());

    // Check timeout
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    if (elapsedTime > timeoutMillis) {
        throw std::runtime_error("Solver timed out");
    }

    // Optimize solution
    optimizeSolution(solution);

    return solution;
}

std::vector<Move> Solver::phase1(const Cube& cube) {
    std::vector<Move> solution;
    
    // Get phase 1 coordinates
    int flip = cube.getFlip();
    int twist = cube.getTwist();
    int slice = cube.getSlice();

    // Check if coordinates are valid
    if (flip >= 2048 || twist >= 2187 || slice >= 495) {
        throw std::runtime_error("Invalid phase 1 coordinates");
    }

    // Try increasing depths until solution found
    for (int depth = 0; depth <= maxDepthPhase1; depth++) {
        if (searchPhase1(flip, twist, slice, depth, solution)) {
            return solution;
        }
    }

    throw std::runtime_error("No phase 1 solution found within depth limit");
}

std::vector<Move> Solver::phase2(const Cube& cube, const std::vector<Move>& phase1Solution) {
    std::vector<Move> solution;
    
    // Get phase 2 coordinates
    int parity = cube.getParity();
    int URFtoDLF = cube.getURFtoDLF();
    int URtoBR = cube.getURtoBR();

    // Check if coordinates are valid
    if (parity >= 2 || URFtoDLF >= 40320 || URtoBR >= 40320) {
        throw std::runtime_error("Invalid phase 2 coordinates");
    }

    // Try increasing depths until solution found
    for (int depth = 0; depth <= maxDepthPhase2; depth++) {
        if (searchPhase2(parity, URFtoDLF, URtoBR, depth, solution)) {
            return solution;
        }
    }

    throw std::runtime_error("No phase 2 solution found within depth limit");
}

bool Solver::searchPhase1(int flip, int twist, int slice, int depth, std::vector<Move>& solution) {
    if (depth == 0) {
        return flip == 0 && twist == 0 && slice == 0;
    }

    // Check pruning tables
    if (std::max({
        flipPrun[flip],
        twistPrun[twist],
        slicePrun[slice]
    }) > depth) {
        return false;
    }

    // Try each move
    for (int moveIdx = 0; moveIdx < 18; moveIdx++) {
        Move move = static_cast<Move>(moveIdx);
        
        // Skip redundant moves
        if (!solution.empty() && !isMovePairAllowed(solution.back(), move)) {
            continue;
        }

        // Apply move
        int newFlip = flipMove[flip][moveIdx];
        int newTwist = twistMove[twist][moveIdx];
        int newSlice = sliceMove[slice][moveIdx];

        // Check if new coordinates are valid
        if (newFlip >= 2048 || newTwist >= 2187 || newSlice >= 495) {
            continue;
        }

        solution.push_back(move);
        if (searchPhase1(newFlip, newTwist, newSlice, depth - 1, solution)) {
            return true;
        }
        solution.pop_back();
    }

    return false;
}

bool Solver::searchPhase2(int parity, int URFtoDLF, int URtoBR, int depth, std::vector<Move>& solution) {
    if (depth == 0) {
        return parity == 0 && URFtoDLF == 0 && URtoBR == 0;
    }

    // Check pruning tables
    if (std::max({
        parityPrun[parity],
        URFtoDLFPrun[URFtoDLF],
        URtoBRPrun[URtoBR]
    }) > depth) {
        return false;
    }

    // Try each move
    for (int moveIdx = 0; moveIdx < 18; moveIdx++) {
        Move move = static_cast<Move>(moveIdx);
        
        // Phase 2 only allows quarter turns on U/D and half turns on all faces
        if (!isPhase2Move(move)) {
            continue;
        }

        // Skip redundant moves
        if (!solution.empty() && !isMovePairAllowed(solution.back(), move)) {
            continue;
        }

        // Apply move
        int newParity = parityMove[parity][moveIdx];
        int newURFtoDLF = URFtoDLFMove[URFtoDLF][moveIdx];
        int newURtoBR = URtoBRMove[URtoBR][moveIdx];

        // Check if new coordinates are valid
        if (newParity >= 2 || newURFtoDLF >= 40320 || newURtoBR >= 40320) {
            continue;
        }

        solution.push_back(move);
        if (searchPhase2(newParity, newURFtoDLF, newURtoBR, depth - 1, solution)) {
            return true;
        }
        solution.pop_back();
    }

    return false;
}

void Solver::initTables() {
    if (!initialized) {
        try {
            initMoveTables();
            initPruningTables();
            initialized = true;
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to initialize tables: " + std::string(e.what()));
        }
    }
}

void Solver::initMoveTables() {
    // Resize tables
    twistMove.resize(2187, std::vector<int>(18));    // 3^7 corner orientations
    flipMove.resize(2048, std::vector<int>(18));     // 2^11 edge orientations
    sliceMove.resize(495, std::vector<int>(18));     // C(12,4) UD-slice edges
    URFtoDLFMove.resize(40320, std::vector<int>(18));// 8! corner permutations
    URtoBRMove.resize(40320, std::vector<int>(18));  // 8! edge permutations
    parityMove.resize(2, std::vector<int>(18));      // 2 possible parities

    // Fill tables by applying moves to reference cubes
    for (int i = 0; i < 2187; i++) {
        Cube cube;
        int twist = i;
        for (int j = 6; j >= 0; j--) {
            int ori = twist % 3;
            twist /= 3;
            cube.co[j] = ori;
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            twistMove[i][j] = moved.getTwist();
        }
    }

    for (int i = 0; i < 2048; i++) {
        Cube cube;
        int flip = i;
        for (int j = 10; j >= 0; j--) {
            int ori = flip & 1;
            flip >>= 1;
            cube.eo[j] = ori;
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            flipMove[i][j] = moved.getFlip();
        }
    }

    for (int i = 0; i < 495; i++) {
        Cube cube;
        int slice = i;
        int k = 0;
        for (int j = 0; j < 12; j++) {
            if (k < 4 && slice < Cnk(11 - j, 3 - k)) {
                cube.ep[j] = static_cast<Cube::Edge>(j + 8);
                k++;
            } else {
                cube.ep[j] = static_cast<Cube::Edge>(j - k);
                slice -= Cnk(11 - j, 3 - k);
            }
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            sliceMove[i][j] = moved.getSlice();
        }
    }

    for (int i = 0; i < 40320; i++) {
        Cube cube;
        int perm = i;
        for (int j = 7; j >= 0; j--) {
            int val = perm % (j + 1);
            perm /= j + 1;
            while (val > 0) {
                cube.cp[j] = cube.cp[j - 1];
                for (int k = j - 1; k > 0; k--) {
                    cube.cp[k] = cube.cp[k - 1];
                }
                val--;
            }
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            URFtoDLFMove[i][j] = moved.getURFtoDLF();
        }
    }

    for (int i = 0; i < 40320; i++) {
        Cube cube;
        int perm = i;
        for (int j = 7; j >= 0; j--) {
            int val = perm % (j + 1);
            perm /= j + 1;
            while (val > 0) {
                cube.ep[j] = cube.ep[j - 1];
                for (int k = j - 1; k > 0; k--) {
                    cube.ep[k] = cube.ep[k - 1];
                }
                val--;
            }
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            URtoBRMove[i][j] = moved.getURtoBR();
        }
    }

    for (int i = 0; i < 2; i++) {
        Cube cube;
        if (i == 1) {
            cube.applyMove(R1);
            cube.applyMove(U1);
        }
        for (int j = 0; j < 18; j++) {
            Cube moved = cube;
            moved.applyMove(static_cast<Move>(j));
            parityMove[i][j] = moved.getParity();
        }
    }
}

void Solver::initPruningTables() {
    // Resize pruning tables
    twistPrun.resize(2187);    // 3^7 corner orientations
    flipPrun.resize(2048);     // 2^11 edge orientations
    slicePrun.resize(495);     // C(12,4) UD-slice edges
    parityPrun.resize(2);      // 2 possible parities
    URFtoDLFPrun.resize(40320);// 8! corner permutations
    URtoBRPrun.resize(40320);  // 8! edge permutations

    // Initialize all entries to -1 (not visited)
    std::fill(twistPrun.begin(), twistPrun.end(), -1);
    std::fill(flipPrun.begin(), flipPrun.end(), -1);
    std::fill(slicePrun.begin(), slicePrun.end(), -1);
    std::fill(parityPrun.begin(), parityPrun.end(), -1);
    std::fill(URFtoDLFPrun.begin(), URFtoDLFPrun.end(), -1);
    std::fill(URtoBRPrun.begin(), URtoBRPrun.end(), -1);

    // Initialize solved state
    twistPrun[0] = 0;
    flipPrun[0] = 0;
    slicePrun[0] = 0;
    parityPrun[0] = 0;
    URFtoDLFPrun[0] = 0;
    URtoBRPrun[0] = 0;

    // Fill pruning tables using breadth-first search
    int depth = 0;
    int done;

    // Phase 1 pruning tables
    do {
        done = 0;
        for (int i = 0; i < 2187; i++) {
            if (twistPrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    int next = twistMove[i][j];
                    if (next < 2187 && twistPrun[next] == -1) {
                        twistPrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);

    depth = 0;
    do {
        done = 0;
        for (int i = 0; i < 2048; i++) {
            if (flipPrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    int next = flipMove[i][j];
                    if (next < 2048 && flipPrun[next] == -1) {
                        flipPrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);

    depth = 0;
    do {
        done = 0;
        for (int i = 0; i < 495; i++) {
            if (slicePrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    int next = sliceMove[i][j];
                    if (next < 495 && slicePrun[next] == -1) {
                        slicePrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);

    // Phase 2 pruning tables
    depth = 0;
    do {
        done = 0;
        for (int i = 0; i < 40320; i++) {
            if (URFtoDLFPrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    if (!isPhase2Move(static_cast<Move>(j))) continue;
                    int next = URFtoDLFMove[i][j];
                    if (next < 40320 && URFtoDLFPrun[next] == -1) {
                        URFtoDLFPrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);

    depth = 0;
    do {
        done = 0;
        for (int i = 0; i < 40320; i++) {
            if (URtoBRPrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    if (!isPhase2Move(static_cast<Move>(j))) continue;
                    int next = URtoBRMove[i][j];
                    if (next < 40320 && URtoBRPrun[next] == -1) {
                        URtoBRPrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);

    depth = 0;
    do {
        done = 0;
        for (int i = 0; i < 2; i++) {
            if (parityPrun[i] == depth) {
                for (int j = 0; j < 18; j++) {
                    if (!isPhase2Move(static_cast<Move>(j))) continue;
                    int next = parityMove[i][j];
                    if (next < 2 && parityPrun[next] == -1) {
                        parityPrun[next] = depth + 1;
                        done++;
                    }
                }
            }
        }
        depth++;
    } while (done > 0);
}

void Solver::optimizeSolution(std::vector<Move>& solution) {
    // Remove redundant move sequences
    bool changed;
    do {
        changed = false;
        
        // Remove cancelling moves (e.g., R R')
        for (size_t i = 0; i < solution.size() - 1; i++) {
            if (areMovesCancelling(solution[i], solution[i + 1])) {
                solution.erase(solution.begin() + i, solution.begin() + i + 2);
                changed = true;
                break;
            }
        }

        // Combine same face moves (e.g., R R -> R2)
        for (size_t i = 0; i < solution.size() - 1; i++) {
            if (areMovesCombinable(solution[i], solution[i + 1])) {
                solution[i] = combineMoves(solution[i], solution[i + 1]);
                solution.erase(solution.begin() + i + 1);
                changed = true;
                break;
            }
        }
    } while (changed);
}

bool Solver::isMovePairAllowed(Move m1, Move m2) {
    // Don't allow moves on same face consecutively
    return (m1 / 3) != (m2 / 3);
}

bool Solver::isPhase2Move(Move move) {
    // Phase 2 only allows:
    // - Quarter turns (U1, U3, D1, D3)
    // - Half turns (U2, D2, R2, L2, F2, B2)
    int face = move / 3;  // 0=U, 1=R, 2=F, 3=D, 4=L, 5=B
    int amount = move % 3;// 0=clockwise, 1=half, 2=counterclockwise
    
    if (face == 0 || face == 3) {  // U or D face
        return true;
    }
    return amount == 1;  // half turn
}

bool Solver::areMovesCancelling(Move m1, Move m2) {
    return (m1 / 3) == (m2 / 3) && ((m1 % 3) + (m2 % 3)) == 3;
}

bool Solver::areMovesCombinable(Move m1, Move m2) {
    return (m1 / 3) == (m2 / 3);
}

Move Solver::combineMoves(Move m1, Move m2) {
    int face = m1 / 3;
    int amount = ((m1 % 3) + (m2 % 3)) % 4;
    if (amount == 3) amount = 1;
    return static_cast<Move>(face * 3 + amount);
}

} // namespace kociemba 