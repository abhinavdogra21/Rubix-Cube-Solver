#include "custom_cube_solver.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <cstring>

// CubeState implementation
CustomCubeSolver::CubeState::CubeState() {
    // Initialize solved cube: each face has 9 stickers of the same color
    // Faces: 0=U(white), 1=R(red), 2=F(green), 3=D(yellow), 4=L(orange), 5=B(blue)
    for (int face = 0; face < 6; face++) {
        for (int sticker = 0; sticker < 9; sticker++) {
            stickers[face * 9 + sticker] = face;
        }
    }
}

bool CustomCubeSolver::CubeState::isSolved() const {
    for (int face = 0; face < 6; face++) {
        for (int sticker = 0; sticker < 9; sticker++) {
            if (stickers[face * 9 + sticker] != face) {
                return false;
            }
        }
    }
    return true;
}

std::string CustomCubeSolver::CubeState::toString() const {
    std::string result;
    for (int i = 0; i < 54; i++) {
        result += std::to_string(stickers[i]);
    }
    return result;
}

bool CustomCubeSolver::CubeState::operator==(const CubeState& other) const {
    return stickers == other.stickers;
}

// CustomCubeSolver implementation
CustomCubeSolver::CustomCubeSolver() {
    initializeMaps();
}

CustomCubeSolver::~CustomCubeSolver() {
}

void CustomCubeSolver::initializeMaps() {
    // Initialize move mappings
    moveMap["U"] = U; moveMap["U'"] = U_PRIME; moveMap["U2"] = U2;
    moveMap["R"] = R; moveMap["R'"] = R_PRIME; moveMap["R2"] = R2;
    moveMap["F"] = F; moveMap["F'"] = F_PRIME; moveMap["F2"] = F2;
    moveMap["D"] = D; moveMap["D'"] = D_PRIME; moveMap["D2"] = D2;
    moveMap["L"] = L; moveMap["L'"] = L_PRIME; moveMap["L2"] = L2;
    moveMap["B"] = B; moveMap["B'"] = B_PRIME; moveMap["B2"] = B2;
    
    // Reverse mapping
    for (const auto& pair : moveMap) {
        reverseMoveMap[pair.second] = pair.first;
    }
    
    // Inverse move mapping
    inverseMoveMap["U"] = "U'"; inverseMoveMap["U'"] = "U"; inverseMoveMap["U2"] = "U2";
    inverseMoveMap["R"] = "R'"; inverseMoveMap["R'"] = "R"; inverseMoveMap["R2"] = "R2";
    inverseMoveMap["F"] = "F'"; inverseMoveMap["F'"] = "F"; inverseMoveMap["F2"] = "F2";
    inverseMoveMap["D"] = "D'"; inverseMoveMap["D'"] = "D"; inverseMoveMap["D2"] = "D2";
    inverseMoveMap["L"] = "L'"; inverseMoveMap["L'"] = "L"; inverseMoveMap["L2"] = "L2";
    inverseMoveMap["B"] = "B'"; inverseMoveMap["B'"] = "B"; inverseMoveMap["B2"] = "B2";
}

std::string CustomCubeSolver::solve(const std::string& scramble) {
    try {
        return solveByInverse(scramble);
    } catch (const std::exception& e) {
        return "Error: " + std::string(e.what());
    }
}

std::string CustomCubeSolver::solveByInverse(const std::string& scramble) {
    std::vector<std::string> moves = parseMoves(scramble);
    std::vector<std::string> solutionMoves;
    
    // Reverse the order and invert each move
    for (int i = moves.size() - 1; i >= 0; i--) {
        std::string move = moves[i];
        if (inverseMoveMap.find(move) != inverseMoveMap.end()) {
            solutionMoves.push_back(inverseMoveMap[move]);
        } else {
            solutionMoves.push_back(move); // Fallback
        }
    }
    
    // Join moves with spaces
    std::string result;
    for (size_t i = 0; i < solutionMoves.size(); i++) {
        if (i > 0) result += " ";
        result += solutionMoves[i];
    }
    
    return result;
}

std::string CustomCubeSolver::generateScramble(int length) {
    std::vector<std::string> moves = {"U", "U'", "U2", "R", "R'", "R2", "F", "F'", "F2",
                                     "D", "D'", "D2", "L", "L'", "L2", "B", "B'", "B2"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, moves.size() - 1);
    
    std::vector<std::string> scrambleMoves;
    for (int i = 0; i < length; i++) {
        scrambleMoves.push_back(moves[dis(gen)]);
    }
    
    std::string result;
    for (size_t i = 0; i < scrambleMoves.size(); i++) {
        if (i > 0) result += " ";
        result += scrambleMoves[i];
    }
    
    return result;
}

bool CustomCubeSolver::isValidScramble(const std::string& scramble) {
    try {
        std::vector<std::string> moves = parseMoves(scramble);
        for (const std::string& move : moves) {
            if (moveMap.find(move) == moveMap.end()) {
                return false;
            }
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> CustomCubeSolver::parseMoves(const std::string& moveString) {
    std::vector<std::string> moves;
    std::istringstream iss(moveString);
    std::string move;
    
    while (iss >> move) {
        moves.push_back(move);
    }
    
    return moves;
}

CustomCubeSolver::Move CustomCubeSolver::stringToMove(const std::string& moveStr) {
    auto it = moveMap.find(moveStr);
    if (it != moveMap.end()) {
        return it->second;
    }
    throw std::invalid_argument("Invalid move: " + moveStr);
}

std::string CustomCubeSolver::moveToString(Move move) {
    auto it = reverseMoveMap.find(move);
    if (it != reverseMoveMap.end()) {
        return it->second;
    }
    return "Unknown";
}

std::string CustomCubeSolver::getInverseMove(const std::string& move) {
    auto it = inverseMoveMap.find(move);
    if (it != inverseMoveMap.end()) {
        return it->second;
    }
    return move; // Fallback
}

void CustomCubeSolver::applyMove(CubeState& cube, Move move) {
    switch (move) {
        case U: applyU(cube); break;
        case U_PRIME: applyU(cube); applyU(cube); applyU(cube); break;
        case U2: applyU(cube); applyU(cube); break;
        case R: applyR(cube); break;
        case R_PRIME: applyR(cube); applyR(cube); applyR(cube); break;
        case R2: applyR(cube); applyR(cube); break;
        case F: applyF(cube); break;
        case F_PRIME: applyF(cube); applyF(cube); applyF(cube); break;
        case F2: applyF(cube); applyF(cube); break;
        case D: applyD(cube); break;
        case D_PRIME: applyD(cube); applyD(cube); applyD(cube); break;
        case D2: applyD(cube); applyD(cube); break;
        case L: applyL(cube); break;
        case L_PRIME: applyL(cube); applyL(cube); applyL(cube); break;
        case L2: applyL(cube); applyL(cube); break;
        case B: applyB(cube); break;
        case B_PRIME: applyB(cube); applyB(cube); applyB(cube); break;
        case B2: applyB(cube); applyB(cube); break;
    }
}

void CustomCubeSolver::rotateFace(CubeState& cube, int face) {
    int base = face * 9;
    std::array<int, 9> temp;
    
    // Copy current face
    for (int i = 0; i < 9; i++) {
        temp[i] = cube.stickers[base + i];
    }
    
    // Rotate face clockwise: 0->2, 1->5, 2->8, 3->1, 4->4, 5->7, 6->0, 7->3, 8->6
    cube.stickers[base + 0] = temp[6];
    cube.stickers[base + 1] = temp[3];
    cube.stickers[base + 2] = temp[0];
    cube.stickers[base + 3] = temp[7];
    cube.stickers[base + 4] = temp[4];
    cube.stickers[base + 5] = temp[1];
    cube.stickers[base + 6] = temp[8];
    cube.stickers[base + 7] = temp[5];
    cube.stickers[base + 8] = temp[2];
}

void CustomCubeSolver::cycleFourPositions(CubeState& cube, int pos1, int pos2, int pos3, int pos4) {
    int temp = cube.stickers[pos1];
    cube.stickers[pos1] = cube.stickers[pos4];
    cube.stickers[pos4] = cube.stickers[pos3];
    cube.stickers[pos3] = cube.stickers[pos2];
    cube.stickers[pos2] = temp;
}

void CustomCubeSolver::applyU(CubeState& cube) {
    rotateFace(cube, 0); // Rotate U face
    // Cycle adjacent edges: R top -> F top -> L top -> B top -> R top
    cycleFourPositions(cube, 9, 18, 36, 45);   // positions 0 of each side
    cycleFourPositions(cube, 10, 19, 37, 46);  // positions 1 of each side
    cycleFourPositions(cube, 11, 20, 38, 47);  // positions 2 of each side
}

void CustomCubeSolver::applyR(CubeState& cube) {
    rotateFace(cube, 1); // Rotate R face
    // Cycle adjacent edges: U right -> F right -> D right -> B left -> U right
    cycleFourPositions(cube, 2, 20, 29, 47);   // right edge of U, F, D and left edge of B
    cycleFourPositions(cube, 5, 23, 32, 44);   // middle right
    cycleFourPositions(cube, 8, 26, 35, 41);   // bottom right
}

void CustomCubeSolver::applyF(CubeState& cube) {
    rotateFace(cube, 2); // Rotate F face
    // Cycle adjacent edges: U bottom -> L right -> D top -> R left -> U bottom
    cycleFourPositions(cube, 6, 38, 33, 11);   // bottom of U, right of L, top of D, left of R
    cycleFourPositions(cube, 7, 41, 30, 14);   // middle
    cycleFourPositions(cube, 8, 44, 27, 17);   // corner
}

void CustomCubeSolver::applyD(CubeState& cube) {
    rotateFace(cube, 3); // Rotate D face
    // Cycle adjacent edges: F bottom -> R bottom -> B bottom -> L bottom -> F bottom
    cycleFourPositions(cube, 24, 15, 51, 42);  // bottom edges
    cycleFourPositions(cube, 25, 16, 52, 43);  // middle bottom
    cycleFourPositions(cube, 26, 17, 53, 44);  // corner bottom
}

void CustomCubeSolver::applyL(CubeState& cube) {
    rotateFace(cube, 4); // Rotate L face
    // Cycle adjacent edges: U left -> B right -> D left -> F left -> U left
    cycleFourPositions(cube, 0, 45, 35, 18);   // left edges
    cycleFourPositions(cube, 3, 48, 32, 21);   // middle left
    cycleFourPositions(cube, 6, 51, 29, 24);   // corner left
}

void CustomCubeSolver::applyB(CubeState& cube) {
    rotateFace(cube, 5); // Rotate B face
    // Cycle adjacent edges: U top -> R right -> D bottom -> L left -> U top
    cycleFourPositions(cube, 0, 9, 35, 36);    // top edges
    cycleFourPositions(cube, 1, 12, 34, 39);   // middle
    cycleFourPositions(cube, 2, 15, 33, 42);   // corner
}

// C interface for Python binding
static CustomCubeSolver* solver = nullptr;
static std::string lastResult;

extern "C" {
    const char* solve_cube(const char* scramble) {
        if (!solver) {
            solver = new CustomCubeSolver();
        }
        lastResult = solver->solve(std::string(scramble));
        return lastResult.c_str();
    }
    
    const char* generate_scramble() {
        if (!solver) {
            solver = new CustomCubeSolver();
        }
        lastResult = solver->generateScramble();
        return lastResult.c_str();
    }
    
    int is_valid_scramble(const char* scramble) {
        if (!solver) {
            solver = new CustomCubeSolver();
        }
        return solver->isValidScramble(std::string(scramble)) ? 1 : 0;
    }
}

