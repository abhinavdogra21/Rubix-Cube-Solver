#pragma once

#include <vector>
#include <string>
#include "cube.h"

namespace kociemba {

class Solver {
public:
    Solver();

    // Main solve function
    std::vector<Move> solve(const std::string& state);

    // Configuration
    void setMaxDepth(int phase1, int phase2) {
        maxDepthPhase1 = phase1;
        maxDepthPhase2 = phase2;
    }
    
    void setTimeout(int milliseconds) {
        timeoutMillis = milliseconds;
    }

private:
    // Phase 1 and 2 search
    std::vector<Move> phase1(const Cube& cube);
    std::vector<Move> phase2(const Cube& cube, const std::vector<Move>& phase1Solution);
    bool searchPhase1(int flip, int twist, int slice, int depth, std::vector<Move>& solution);
    bool searchPhase2(int parity, int URFtoDLF, int URtoBR, int depth, std::vector<Move>& solution);

    // Move sequence optimization
    void optimizeSolution(std::vector<Move>& solution);
    bool isMovePairAllowed(Move m1, Move m2);
    bool isPhase2Move(Move move);
    bool areMovesCancelling(Move m1, Move m2);
    bool areMovesCombinable(Move m1, Move m2);
    Move combineMoves(Move m1, Move m2);

    // Table initialization
    void initTables();
    void initMoveTables();
    void initPruningTables();

    // Move tables
    static std::vector<std::vector<int>> twistMove;
    static std::vector<std::vector<int>> flipMove;
    static std::vector<std::vector<int>> sliceMove;
    static std::vector<std::vector<int>> URFtoDLFMove;
    static std::vector<std::vector<int>> URtoBRMove;
    static std::vector<std::vector<int>> parityMove;

    // Pruning tables
    static std::vector<int> twistPrun;
    static std::vector<int> flipPrun;
    static std::vector<int> slicePrun;
    static std::vector<int> parityPrun;
    static std::vector<int> URFtoDLFPrun;
    static std::vector<int> URtoBRPrun;

    // Configuration
    int maxDepthPhase1;
    int maxDepthPhase2;
    int timeoutMillis;
    bool initialized;
};

} // namespace kociemba 