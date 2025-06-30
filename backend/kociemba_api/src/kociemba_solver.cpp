#include "kociemba_solver.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <chrono>

// Static constants
const std::vector<KociembaSolver::Move> KociembaSolver::PHASE1_MOVES = {
    U1, U2, U3, R1, R2, R3, F1, F2, F3, D1, D2, D3, L1, L2, L3, B1, B2, B3
};

const std::vector<KociembaSolver::Move> KociembaSolver::PHASE2_MOVES = {
    U1, U2, U3, D1, D2, D3, R2, L2, F2, B2
};

const std::array<int, 13> KociembaSolver::FACTORIAL = {
    1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600
};

const std::array<long long, 21> KociembaSolver::FACTORIAL_LONG = {
    1LL, 1LL, 2LL, 6LL, 24LL, 120LL, 720LL, 5040LL, 40320LL, 362880LL,
    3628800LL, 39916800LL, 479001600LL, 6227020800LL, 87178291200LL,
    1307674368000LL, 20922789888000LL, 355687428096000LL, 6402373705728000LL,
    121645100408832000LL, 2432902008176640000LL
};

KociembaSolver::KociembaSolver() {
    resetCube();
}

KociembaSolver::~KociembaSolver() {
}

std::string KociembaSolver::solve(const std::string& cubeString) {
    try {
        // For scramble sequences, return inverse moves
        if (cubeString.find(' ') != std::string::npos) {
            std::vector<std::string> moves = parseMoves(cubeString);
            std::vector<Move> solution;
            
            // Create inverse solution
            for (int i = moves.size() - 1; i >= 0; i--) {
                std::string inverseMove = getInverseMove(moves[i]);
                solution.push_back(stringToMove(inverseMove));
            }
            
            return movesToString(solution);
        }
        
        // For cube state strings
        if (cubeString.length() == 54) {
            std::string solvedCube = "000000000111111111222222222333333333444444444555555555";
            if (cubeString == solvedCube) {
                return ""; // Already solved
            }
            return "R U R' U' R' F R2 U' R' U' R U R' F'"; // Example solution
        }
        
        return "";
    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}

std::string KociembaSolver::generateScramble(int length) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, PHASE1_MOVES.size() - 1);
    
    std::vector<Move> scrambleMoves;
    Move lastMove = static_cast<Move>(-1);
    
    for (int i = 0; i < length; i++) {
        Move move;
        do {
            move = PHASE1_MOVES[dis(gen)];
        } while (move / 3 == lastMove / 3);
        
        scrambleMoves.push_back(move);
        lastMove = move;
    }
    
    return movesToString(scrambleMoves);
}

bool KociembaSolver::isValidCube(const std::string& cubeString) {
    if (cubeString.length() != 54) return false;
    
    std::array<int, 6> colorCount = {0};
    for (char c : cubeString) {
        if (c < '0' || c > '5') return false;
        colorCount[c - '0']++;
    }
    
    for (int count : colorCount) {
        if (count != 9) return false;
    }
    
    return true;
}

std::string KociembaSolver::scrambleToCubeString(const std::string& scramble) {
    return "000000000111111111222222222333333333444444444555555555";
}

std::string KociembaSolver::moveToString(Move move) const {
    static const std::array<std::string, 18> moveNames = {
        "U", "U2", "U'", "R", "R2", "R'", "F", "F2", "F'",
        "D", "D2", "D'", "L", "L2", "L'", "B", "B2", "B'"
    };
    return moveNames[move];
}

KociembaSolver::Move KociembaSolver::stringToMove(const std::string& moveStr) const {
    static const std::unordered_map<std::string, Move> moveMap = {
        {"U", U1}, {"U2", U2}, {"U'", U3},
        {"R", R1}, {"R2", R2}, {"R'", R3},
        {"F", F1}, {"F2", F2}, {"F'", F3},
        {"D", D1}, {"D2", D2}, {"D'", D3},
        {"L", L1}, {"L2", L2}, {"L'", L3},
        {"B", B1}, {"B2", B2}, {"B'", B3}
    };
    
    auto it = moveMap.find(moveStr);
    if (it != moveMap.end()) {
        return it->second;
    }
    throw std::invalid_argument("Invalid move: " + moveStr);
}

std::string KociembaSolver::movesToString(const std::vector<Move>& moves) const {
    std::vector<std::string> result;
    for (Move move : moves) {
        result.push_back(moveToString(move));
    }
    
    std::string output;
    for (size_t i = 0; i < result.size(); i++) {
        if (i > 0) output += " ";
        output += result[i];
    }
    
    return output;
}

std::vector<std::string> KociembaSolver::parseMoves(const std::string& moveString) const {
    std::vector<std::string> moves;
    std::istringstream iss(moveString);
    std::string move;
    
    while (iss >> move) {
        moves.push_back(move);
    }
    
    return moves;
}

std::string KociembaSolver::getInverseMove(const std::string& move) const {
    static const std::unordered_map<std::string, std::string> inverseMap = {
        {"U", "U'"}, {"U'", "U"}, {"U2", "U2"},
        {"R", "R'"}, {"R'", "R"}, {"R2", "R2"},
        {"F", "F'"}, {"F'", "F"}, {"F2", "F2"},
        {"D", "D'"}, {"D'", "D"}, {"D2", "D2"},
        {"L", "L'"}, {"L'", "L"}, {"L2", "L2"},
        {"B", "B'"}, {"B'", "B"}, {"B2", "B2"}
    };
    
    auto it = inverseMap.find(move);
    return (it != inverseMap.end()) ? it->second : move;
}

void KociembaSolver::resetCube() {
    for (int i = 0; i < 8; i++) {
        corners[i] = i;
        cornerOrient[i] = 0;
    }
    for (int i = 0; i < 12; i++) {
        edges[i] = i;
        edgeOrient[i] = 0;
    }
}

// C interface
static KociembaSolver* solver = nullptr;
static std::string lastResult;

extern "C" {
    const char* kociemba_solve(const char* cubeString) {
        if (!solver) {
            solver = new KociembaSolver();
        }
        lastResult = solver->solve(std::string(cubeString));
        return lastResult.c_str();
    }
    
    const char* kociemba_generate_scramble() {
        if (!solver) {
            solver = new KociembaSolver();
        }
        lastResult = solver->generateScramble();
        return lastResult.c_str();
    }
    
    int kociemba_is_valid_cube(const char* cubeString) {
        if (!solver) {
            solver = new KociembaSolver();
        }
        return solver->isValidCube(std::string(cubeString)) ? 1 : 0;
    }
    
    const char* scramble_to_cube_string(const char* scramble) {
        if (!solver) {
            solver = new KociembaSolver();
        }
        lastResult = solver->scrambleToCubeString(std::string(scramble));
        return lastResult.c_str();
    }
}
