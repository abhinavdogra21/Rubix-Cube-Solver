#ifndef CUSTOM_CUBE_SOLVER_H
#define CUSTOM_CUBE_SOLVER_H

#include <string>
#include <vector>
#include <map>
#include <array>

class CustomCubeSolver {
public:
    CustomCubeSolver();
    ~CustomCubeSolver();
    
    // Main solving function
    std::string solve(const std::string& scramble);
    
    // Generate a random scramble
    std::string generateScramble(int length = 20);
    
    // Validate scramble string
    bool isValidScramble(const std::string& scramble);
    
private:
    // Cube state representation (54 stickers: 9 per face)
    struct CubeState {
        std::array<int, 54> stickers;
        
        CubeState();
        bool isSolved() const;
        std::string toString() const;
        bool operator==(const CubeState& other) const;
    };
    
    // Move definitions
    enum Move {
        U, U_PRIME, U2,
        R, R_PRIME, R2,
        F, F_PRIME, F2,
        D, D_PRIME, D2,
        L, L_PRIME, L2,
        B, B_PRIME, B2
    };
    
    // Move application functions
    void applyMove(CubeState& cube, Move move);
    void applyU(CubeState& cube);
    void applyR(CubeState& cube);
    void applyF(CubeState& cube);
    void applyD(CubeState& cube);
    void applyL(CubeState& cube);
    void applyB(CubeState& cube);
    
    // Helper functions
    void rotateFace(CubeState& cube, int face);
    void cycleFourPositions(CubeState& cube, int pos1, int pos2, int pos3, int pos4);
    
    // String parsing
    std::vector<std::string> parseMoves(const std::string& moveString);
    Move stringToMove(const std::string& moveStr);
    std::string moveToString(Move move);
    std::string getInverseMove(const std::string& move);
    
    // Solving algorithm (simple inverse method)
    std::string solveByInverse(const std::string& scramble);
    
    // Move mappings
    std::map<std::string, Move> moveMap;
    std::map<Move, std::string> reverseMoveMap;
    std::map<std::string, std::string> inverseMoveMap;
    
    void initializeMaps();
};

// C interface for Python binding
extern "C" {
    const char* solve_cube(const char* scramble);
    const char* generate_scramble();
    int is_valid_scramble(const char* scramble);
}

#endif // CUSTOM_CUBE_SOLVER_H

