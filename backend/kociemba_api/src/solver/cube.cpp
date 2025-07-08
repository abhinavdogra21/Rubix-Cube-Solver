#include "cube.h"
#include <stdexcept>
#include <sstream>
#include <map>
#include <array>
#include <algorithm>

namespace kociemba {

// Helper functions
int factorial(int n) {
    static const int factorials[] = {
        1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800
    };
    return factorials[n];
}

int Cnk(int n, int k) {
    if (n < k || k < 0) return 0;
    if (k > n/2) k = n - k;
    int s = 1;
    for (int i = 0; i < k; i++) {
        s *= n - i;
        s /= i + 1;
    }
    return s;
}

// Initialize move tables
const std::array<std::array<Cube::Corner, 8>, 18> Cube::cornerMove = Cube::initCornerMove();
const std::array<std::array<int, 8>, 18> Cube::cornerOrient = Cube::initCornerOrient();
const std::array<std::array<Cube::Edge, 12>, 18> Cube::edgeMove = Cube::initEdgeMove();
const std::array<std::array<int, 12>, 18> Cube::edgeOrient = Cube::initEdgeOrient();

Cube::Cube() {
    // Initialize solved state
    for (int i = 0; i < 8; i++) {
        cp[i] = static_cast<Corner>(i);
        co[i] = 0;
    }
    for (int i = 0; i < 12; i++) {
        ep[i] = static_cast<Edge>(i);
        eo[i] = 0;
    }
}

Cube::Cube(const std::string& state) {
    if (!isValidState(state)) {
        throw std::invalid_argument("Invalid cube state");
    }
    initFromString(state);
}

void Cube::applyMove(Move move) {
    // Apply corner permutation and orientation
    std::array<Corner, 8> newCp = cp;
    std::array<int, 8> newCo = co;
    for (int i = 0; i < 8; i++) {
        newCp[i] = cornerMove[move][i];
        newCo[i] = (co[cornerMove[move][i]] + cornerOrient[move][i]) % 3;
    }
    cp = newCp;
    co = newCo;

    // Apply edge permutation and orientation
    std::array<Edge, 12> newEp = ep;
    std::array<int, 12> newEo = eo;
    for (int i = 0; i < 12; i++) {
        newEp[i] = edgeMove[move][i];
        newEo[i] = (eo[edgeMove[move][i]] + edgeOrient[move][i]) % 2;
    }
    ep = newEp;
    eo = newEo;
}

bool Cube::isSolved() const {
    // Check corner positions and orientations
    for (int i = 0; i < 8; i++) {
        if (cp[i] != static_cast<Corner>(i) || co[i] != 0) {
            return false;
        }
    }

    // Check edge positions and orientations
    for (int i = 0; i < 12; i++) {
        if (ep[i] != static_cast<Edge>(i) || eo[i] != 0) {
            return false;
        }
    }

    return true;
}

std::string Cube::toString() const {
    // Convert cube state to string representation
    std::string result(54, ' ');
    
    // Map corner positions and orientations to facelet positions
    static const std::array<std::array<int, 3>, 8> cornerFacelet = {{
        {{8, 9, 20}},   // URF
        {{6, 18, 38}},  // UFL
        {{0, 36, 47}},  // ULB
        {{2, 45, 11}},  // UBR
        {{29, 26, 15}}, // DFR
        {{27, 44, 24}}, // DLF
        {{33, 53, 42}}, // DBL
        {{35, 17, 51}}  // DRB
    }};

    // Map edge positions and orientations to facelet positions
    static const std::array<std::array<int, 2>, 12> edgeFacelet = {{
        {{5, 10}},   // UR
        {{7, 19}},   // UF
        {{3, 37}},   // UL
        {{1, 46}},   // UB
        {{32, 16}},  // DR
        {{28, 25}},  // DF
        {{30, 43}},  // DL
        {{34, 52}},  // DB
        {{23, 12}},  // FR
        {{21, 41}},  // FL
        {{39, 50}},  // BL
        {{48, 14}}   // BR
    }};

    // Fill centers
    result[4] = 'U';   // U center
    result[13] = 'R';  // R center
    result[22] = 'F';  // F center
    result[31] = 'D';  // D center
    result[40] = 'L';  // L center
    result[49] = 'B';  // B center

    // Fill corners
    for (int i = 0; i < 8; i++) {
        int ori = co[i];
        for (int j = 0; j < 3; j++) {
            int pos = cornerFacelet[i][(j + ori) % 3];
            char color;
            switch ((j + ori) % 3) {
                case 0: color = "URFDLB"[cp[i] / 3]; break;
                case 1: color = "URFDLB"[(cp[i] + 1) % 6]; break;
                case 2: color = "URFDLB"[(cp[i] + 2) % 6]; break;
            }
            result[pos] = color;
        }
    }

    // Fill edges
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 2; j++) {
            int pos = edgeFacelet[i][(j + eo[i]) % 2];
            char color;
            if ((j + eo[i]) % 2 == 0) {
                color = "URFDLB"[ep[i] / 3];
            } else {
                color = "URFDLB"[(ep[i] + 1) % 6];
            }
            result[pos] = color;
        }
    }

    return result;
}

bool Cube::isValidState(const std::string& state) {
    if (state.length() != 54) return false;

    // Count colors
    std::map<char, int> colorCount;
    for (char c : state) {
        if (c != 'U' && c != 'R' && c != 'F' && 
            c != 'D' && c != 'L' && c != 'B') {
            return false;
        }
        colorCount[c]++;
    }

    // Check if each color appears exactly 9 times
    for (const auto& pair : colorCount) {
        if (pair.second != 9) return false;
    }

    return true;
}

std::string Cube::moveToString(Move move) {
    static const char* faces = "URFDLB";
    static const char* amounts = " 2'";
    return std::string(1, faces[move / 3]) + amounts[move % 3];
}

Move Cube::stringToMove(const std::string& str) {
    if (str.length() < 1 || str.length() > 2) {
        throw std::invalid_argument("Invalid move string");
    }

    // Get face
    char face = str[0];
    int faceIdx;
    switch (face) {
        case 'U': faceIdx = 0; break;
        case 'R': faceIdx = 1; break;
        case 'F': faceIdx = 2; break;
        case 'D': faceIdx = 3; break;
        case 'L': faceIdx = 4; break;
        case 'B': faceIdx = 5; break;
        default: throw std::invalid_argument("Invalid face");
    }

    // Get amount
    int amount;
    if (str.length() == 1) {
        amount = 0;  // clockwise
    } else {
        switch (str[1]) {
            case '2': amount = 1; break;  // half turn
            case '\'': amount = 2; break; // counterclockwise
            default: throw std::invalid_argument("Invalid amount");
        }
    }

    return static_cast<Move>(faceIdx * 3 + amount);
}

void Cube::initFromString(const std::string& state) {
    // Initialize arrays
    std::array<Corner, 8> newCp;
    std::array<int, 8> newCo;
    std::array<Edge, 12> newEp;
    std::array<int, 12> newEo;

    // Map from color to face index (0=U, 1=R, 2=F, 3=D, 4=L, 5=B)
    std::array<int, 128> colorToFace;
    colorToFace['U'] = 0;
    colorToFace['R'] = 1;
    colorToFace['F'] = 2;
    colorToFace['D'] = 3;
    colorToFace['L'] = 4;
    colorToFace['B'] = 5;

    // Corner facelet positions
    static const std::array<std::array<int, 3>, 8> cornerFacelet = {{
        {{8, 9, 20}},   // URF
        {{6, 18, 38}},  // UFL
        {{0, 36, 47}},  // ULB
        {{2, 45, 11}},  // UBR
        {{29, 26, 15}}, // DFR
        {{27, 44, 24}}, // DLF
        {{33, 53, 42}}, // DBL
        {{35, 17, 51}}  // DRB
    }};

    // Corner color arrangements
    static const std::array<std::array<int, 3>, 8> cornerColor = {{
        {{0, 1, 2}}, // URF
        {{0, 2, 4}}, // UFL
        {{0, 4, 5}}, // ULB
        {{0, 5, 1}}, // UBR
        {{3, 2, 1}}, // DFR
        {{3, 4, 2}}, // DLF
        {{3, 5, 4}}, // DBL
        {{3, 1, 5}}  // DRB
    }};

    // Find corner positions and orientations
    for (int i = 0; i < 8; i++) {
        // Get colors of this corner
        std::array<int, 3> colors = {{
            colorToFace[state[cornerFacelet[i][0]]],
            colorToFace[state[cornerFacelet[i][1]]],
            colorToFace[state[cornerFacelet[i][2]]]
        }};

        // Find matching corner
        for (int j = 0; j < 8; j++) {
            std::array<int, 3> target = cornerColor[j];
            
            // Try all three rotations
            for (int ori = 0; ori < 3; ori++) {
                if (colors[0] == target[(0 + ori) % 3] &&
                    colors[1] == target[(1 + ori) % 3] &&
                    colors[2] == target[(2 + ori) % 3]) {
                    newCp[i] = static_cast<Corner>(j);
                    newCo[i] = ori;
                    break;
                }
            }
        }
    }

    // Edge facelet positions
    static const std::array<std::array<int, 2>, 12> edgeFacelet = {{
        {{5, 10}},   // UR
        {{7, 19}},   // UF
        {{3, 37}},   // UL
        {{1, 46}},   // UB
        {{32, 16}},  // DR
        {{28, 25}},  // DF
        {{30, 43}},  // DL
        {{34, 52}},  // DB
        {{23, 12}},  // FR
        {{21, 41}},  // FL
        {{39, 50}},  // BL
        {{48, 14}}   // BR
    }};

    // Edge color arrangements
    static const std::array<std::array<int, 2>, 12> edgeColor = {{
        {{0, 1}}, // UR
        {{0, 2}}, // UF
        {{0, 4}}, // UL
        {{0, 5}}, // UB
        {{3, 1}}, // DR
        {{3, 2}}, // DF
        {{3, 4}}, // DL
        {{3, 5}}, // DB
        {{2, 1}}, // FR
        {{2, 4}}, // FL
        {{5, 4}}, // BL
        {{5, 1}}  // BR
    }};

    // Find edge positions and orientations
    for (int i = 0; i < 12; i++) {
        // Get colors of this edge
        std::array<int, 2> colors = {{
            colorToFace[state[edgeFacelet[i][0]]],
            colorToFace[state[edgeFacelet[i][1]]]
        }};

        // Find matching edge
        for (int j = 0; j < 12; j++) {
            std::array<int, 2> target = edgeColor[j];
            
            // Try both orientations
            if (colors[0] == target[0] && colors[1] == target[1]) {
                newEp[i] = static_cast<Edge>(j);
                newEo[i] = 0;
                break;
            }
            if (colors[0] == target[1] && colors[1] == target[0]) {
                newEp[i] = static_cast<Edge>(j);
                newEo[i] = 1;
                break;
            }
        }
    }

    // Update cube state
    cp = newCp;
    co = newCo;
    ep = newEp;
    eo = newEo;
}

std::array<std::array<Cube::Corner, 8>, 18> Cube::initCornerMove() {
    std::array<std::array<Corner, 8>, 18> moves;
    
    // Define basic moves
    moves[U1] = {{UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB}};
    moves[R1] = {{DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR}};
    moves[F1] = {{UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB}};
    moves[D1] = {{URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR}};
    moves[L1] = {{URF, DBL, ULB, UBR, DFR, UFL, DLF, DRB}};
    moves[B1] = {{URF, UFL, DRB, UBR, DFR, DLF, ULB, DBL}};
    
    // Generate half turns and counterclockwise moves
    for (int face = 0; face < 6; face++) {
        Move m1 = static_cast<Move>(face * 3);
        Move m2 = static_cast<Move>(face * 3 + 1);
        Move m3 = static_cast<Move>(face * 3 + 2);
        
        // Half turn (m2) = m1 * m1
        for (int i = 0; i < 8; i++) {
            moves[m2][i] = moves[m1][moves[m1][i]];
        }
        
        // Counterclockwise (m3) = m1 * m1 * m1
        for (int i = 0; i < 8; i++) {
            moves[m3][i] = moves[m1][moves[m2][i]];
        }
    }
    
    return moves;
}

std::array<std::array<int, 8>, 18> Cube::initCornerOrient() {
    std::array<std::array<int, 8>, 18> orients;
    
    // Define basic moves
    orients[U1] = {{0, 0, 0, 0, 0, 0, 0, 0}};
    orients[R1] = {{2, 0, 0, 1, 1, 0, 0, 2}};
    orients[F1] = {{1, 2, 0, 0, 2, 1, 0, 0}};
    orients[D1] = {{0, 0, 0, 0, 0, 0, 0, 0}};
    orients[L1] = {{0, 1, 0, 0, 0, 2, 2, 0}};
    orients[B1] = {{0, 0, 1, 2, 0, 0, 2, 1}};
    
    // Generate half turns and counterclockwise moves
    for (int face = 0; face < 6; face++) {
        Move m1 = static_cast<Move>(face * 3);
        Move m2 = static_cast<Move>(face * 3 + 1);
        Move m3 = static_cast<Move>(face * 3 + 2);
        
        // Half turn = m1 + m1
        for (int i = 0; i < 8; i++) {
            orients[m2][i] = (orients[m1][i] + orients[m1][cornerMove[m1][i]]) % 3;
        }
        
        // Counterclockwise = m1 + m1 + m1
        for (int i = 0; i < 8; i++) {
            orients[m3][i] = (orients[m1][i] + orients[m1][cornerMove[m1][i]] + orients[m1][cornerMove[m2][i]]) % 3;
        }
    }
    
    return orients;
}

std::array<std::array<Cube::Edge, 12>, 18> Cube::initEdgeMove() {
    std::array<std::array<Edge, 12>, 18> moves;
    
    // Define basic moves
    moves[U1] = {{UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR}};
    moves[R1] = {{FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR}};
    moves[F1] = {{UF, FL, UL, UB, DR, FR, DL, DB, DF, UF, BL, BR}};
    moves[D1] = {{UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR}};
    moves[L1] = {{UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR}};
    moves[B1] = {{UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB}};
    
    // Generate half turns and counterclockwise moves
    for (int face = 0; face < 6; face++) {
        Move m1 = static_cast<Move>(face * 3);
        Move m2 = static_cast<Move>(face * 3 + 1);
        Move m3 = static_cast<Move>(face * 3 + 2);
        
        // Half turn = m1 * m1
        for (int i = 0; i < 12; i++) {
            moves[m2][i] = moves[m1][moves[m1][i]];
        }
        
        // Counterclockwise = m1 * m1 * m1
        for (int i = 0; i < 12; i++) {
            moves[m3][i] = moves[m1][moves[m2][i]];
        }
    }
    
    return moves;
}

std::array<std::array<int, 12>, 18> Cube::initEdgeOrient() {
    std::array<std::array<int, 12>, 18> orients;
    
    // Define basic moves
    orients[U1] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    orients[R1] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    orients[F1] = {{1, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0}};
    orients[D1] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    orients[L1] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    orients[B1] = {{0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1}};
    
    // Generate half turns and counterclockwise moves
    for (int face = 0; face < 6; face++) {
        Move m1 = static_cast<Move>(face * 3);
        Move m2 = static_cast<Move>(face * 3 + 1);
        Move m3 = static_cast<Move>(face * 3 + 2);
        
        // Half turn = m1 + m1
        for (int i = 0; i < 12; i++) {
            orients[m2][i] = (orients[m1][i] + orients[m1][edgeMove[m1][i]]) % 2;
        }
        
        // Counterclockwise = m1 + m1 + m1
        for (int i = 0; i < 12; i++) {
            orients[m3][i] = (orients[m1][i] + orients[m1][edgeMove[m1][i]] + orients[m1][edgeMove[m2][i]]) % 2;
        }
    }
    
    return orients;
}

int Cube::getTwist() const {
    return computeTwist();
}

int Cube::getFlip() const {
    return computeFlip();
}

int Cube::getSlice() const {
    return computeSlice();
}

int Cube::getParity() const {
    return computeParity();
}

int Cube::getURFtoDLF() const {
    return computeURFtoDLF();
}

int Cube::getURtoBR() const {
    return computeURtoBR();
}

int Cube::computeTwist() const {
    int twist = 0;
    for (int i = 0; i < 7; i++) {
        twist = twist * 3 + co[i];
    }
    return twist;
}

int Cube::computeFlip() const {
    int flip = 0;
    for (int i = 0; i < 11; i++) {
        flip = flip * 2 + eo[i];
    }
    return flip;
}

int Cube::computeSlice() const {
    // Count UD-slice edges in FB-slice
    int slice = 0;
    int x = 0;
    for (int i = 0; i < 12; i++) {
        if (ep[i] >= 8) {
            slice += Cnk(11 - i, x + 1);
            x++;
        }
    }
    return slice;
}

int Cube::computeParity() const {
    int parity = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = i + 1; j < 8; j++) {
            if (cp[i] > cp[j]) parity++;
        }
    }
    return parity % 2;
}

int Cube::computeURFtoDLF() const {
    // Convert corner permutation to index (0-40319)
    int index = 0;
    int r = 7;
    for (int i = 0; i < 8; i++) {
        int v = 0;
        for (int j = i + 1; j < 8; j++) {
            if (cp[j] > cp[i]) v++;
        }
        index += v * factorial(r);
        r--;
    }
    return index;
}

int Cube::computeURtoBR() const {
    // Convert edge permutation to index (0-40319)
    int index = 0;
    int r = 11;
    for (int i = 0; i < 12; i++) {
        int v = 0;
        for (int j = i + 1; j < 12; j++) {
            if (ep[j] > ep[i]) v++;
        }
        index += v * factorial(r);
        r--;
    }
    return index;
}

} // namespace kociemba 