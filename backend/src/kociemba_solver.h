#ifndef KOCIEMBA_SOLVER_H
#define KOCIEMBA_SOLVER_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <sstream>

class KociembaSolver {
public:
    enum Move {
        U1, U2, U3, R1, R2, R3, F1, F2, F3,
        D1, D2, D3, L1, L2, L3, B1, B2, B3
    };

    KociembaSolver();
    ~KociembaSolver();
    
    std::string solve(const std::string& cubeString);
    std::string generateScramble(int length = 25);
    bool isValidCube(const std::string& cubeString);
    std::string scrambleToCubeString(const std::string& scramble);
    
    static const std::vector<Move> PHASE1_MOVES;
    static const std::vector<Move> PHASE2_MOVES;
    static const std::array<int, 13> FACTORIAL;
    static const std::array<long long, 21> FACTORIAL_LONG;

private:
    std::array<int, 8> corners;
    std::array<int, 8> cornerOrient;
    std::array<int, 12> edges;
    std::array<int, 12> edgeOrient;
    
    std::string moveToString(Move move) const;
    Move stringToMove(const std::string& moveStr) const;
    std::string movesToString(const std::vector<Move>& moves) const;
    std::vector<std::string> parseMoves(const std::string& moveString) const;
    std::string getInverseMove(const std::string& move) const;
    void resetCube();
};

extern "C" {
    const char* kociemba_solve(const char* cubeString);
    const char* kociemba_generate_scramble();
    int kociemba_is_valid_cube(const char* cubeString);
    const char* scramble_to_cube_string(const char* scramble);
}

#endif
