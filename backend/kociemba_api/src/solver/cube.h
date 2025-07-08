#pragma once

#include <array>
#include <string>
#include <vector>

namespace kociemba {

// Forward declaration
class Solver;

// Helper functions
int factorial(int n);
int Cnk(int n, int k);

// Face colors
enum Color { U, R, F, D, L, B };

// Move types
enum Move {
    U1, U2, U3, R1, R2, R3, F1, F2, F3,
    D1, D2, D3, L1, L2, L3, B1, B2, B3
};

class Cube {
public:
    // Corner positions
    enum Corner { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB };
    
    // Edge positions
    enum Edge { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR };

    Cube(); // Initialize solved cube
    explicit Cube(const std::string& state); // Initialize from string

    // Core operations
    void applyMove(Move move);
    bool isSolved() const;
    std::string toString() const;

    // Get current state
    std::array<Corner, 8> getCornerPositions() const { return cp; }
    std::array<int, 8> getCornerOrientations() const { return co; }
    std::array<Edge, 12> getEdgePositions() const { return ep; }
    std::array<int, 12> getEdgeOrientations() const { return eo; }

    // Phase 1 coordinates
    int getTwist() const;
    int getFlip() const;
    int getSlice() const;

    // Phase 2 coordinates
    int getParity() const;
    int getURFtoDLF() const;
    int getURtoBR() const;

    // Static helpers
    static bool isValidState(const std::string& state);
    static std::string moveToString(Move move);
    static Move stringToMove(const std::string& str);

private:
    // Cube state
    std::array<Corner, 8> cp;  // corner permutation
    std::array<int, 8> co;     // corner orientation
    std::array<Edge, 12> ep;   // edge permutation
    std::array<int, 12> eo;    // edge orientation

    // Move tables
    static const std::array<std::array<Corner, 8>, 18> cornerMove;
    static const std::array<std::array<int, 8>, 18> cornerOrient;
    static const std::array<std::array<Edge, 12>, 18> edgeMove;
    static const std::array<std::array<int, 12>, 18> edgeOrient;

    // Helper methods
    void initFromString(const std::string& state);
    static std::array<std::array<Corner, 8>, 18> initCornerMove();
    static std::array<std::array<int, 8>, 18> initCornerOrient();
    static std::array<std::array<Edge, 12>, 18> initEdgeMove();
    static std::array<std::array<int, 12>, 18> initEdgeOrient();

    // Coordinate computation helpers
    int computeTwist() const;
    int computeFlip() const;
    int computeSlice() const;
    int computeParity() const;
    int computeURFtoDLF() const;
    int computeURtoBR() const;

    // Make Solver a friend class
    friend class Solver;
};

} // namespace kociemba 