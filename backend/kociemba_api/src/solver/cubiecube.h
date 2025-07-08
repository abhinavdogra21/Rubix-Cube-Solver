#pragma once

#include <array>
#include <cstdint>

// Corner positions
enum Corner {
    URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB
};

// Edge positions
enum Edge {
    UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
};

class CubieCube {
public:
    CubieCube();
    CubieCube(const std::array<Corner, 8>& cp, const std::array<uint8_t, 8>& co,
              const std::array<Edge, 12>& ep, const std::array<uint8_t, 12>& eo);

    // Corner permutation and orientation
    std::array<Corner, 8> cp;
    std::array<uint8_t, 8> co;

    // Edge permutation and orientation
    std::array<Edge, 12> ep;
    std::array<uint8_t, 12> eo;

    // Basic cube operations
    void cornerMultiply(const CubieCube& b);
    void edgeMultiply(const CubieCube& b);
    void multiply(const CubieCube& b);

    // Getters for phase 1 and 2 coordinates
    int getTwist() const;
    int getFlip() const;
    int getURFtoDLF() const;
    int getURtoBR() const;
    int getURtoDF() const;
    int getUBtoDF() const;
    int getCornerParity() const;
    int getEdgeParity() const;

    // Setters for phase 1 and 2 coordinates
    void setTwist(int twist);
    void setFlip(int flip);
    void setURFtoDLF(int idx);
    void setURtoBR(int idx);
    void setURtoDF(int idx);
    void setUBtoDF(int idx);

    // Move tables
    static const std::array<CubieCube, 18> moveCube;
    static const std::array<int, 8> cornerFacelet;
    static const std::array<int, 12> edgeFacelet;

private:
    static std::array<CubieCube, 18> initMoveCube();
}; 