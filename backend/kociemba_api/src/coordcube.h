#pragma once

#include <array>
#include <vector>
#include "cubiecube.h"

// Coordinate representation of the cube
class CoordCube {
public:
    CoordCube();
    CoordCube(int twist, int flip, int slice, int parity, int URFtoDLF, int URtoBR);

    // Phase 1 coordinates
    int twist;      // orientation of corners
    int flip;       // orientation of edges
    int slice;      // position of UD-slice edges

    // Phase 2 coordinates
    int parity;     // parity of edge permutation
    int URFtoDLF;   // permutation of corners
    int URtoBR;     // permutation of edges

    // Move tables
    static std::array<std::array<int, 18>, 2187> twistMove;   // 3^7 corner orientations
    static std::array<std::array<int, 18>, 2048> flipMove;    // 2^11 edge orientations
    static std::array<std::array<int, 18>, 495> sliceMove;    // C(12,4) UD-slice edge positions
    static std::array<std::array<int, 18>, 40320> URFtoDLFMove; // 8! corner permutations
    static std::array<std::array<int, 18>, 479001600> URtoBRMove; // 12! edge permutations

    // Pruning tables
    static std::array<char, 2187 * 2048> twistFlipPrun;
    static std::array<char, 495 * 40320> sliceURFtoDLFPrun;
    static std::array<char, 495 * 479001600> sliceURtoBRPrun;

    // Initialize all move and pruning tables
    static void initTables();

private:
    // Helper methods for table initialization
    static void initTwistMove();
    static void initFlipMove();
    static void initSliceMove();
    static void initURFtoDLFMove();
    static void initURtoBRMove();
    static void initTwistFlipPrun();
    static void initSliceURFtoDLFPrun();
    static void initSliceURtoBRPrun();
}; 