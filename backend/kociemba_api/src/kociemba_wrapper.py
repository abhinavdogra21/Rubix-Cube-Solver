"""
Python wrapper for the Kociemba two-phase algorithm C++ solver
"""

import ctypes
import os
import sys

class KociembaSolverWrapper:
    def __init__(self):
        self.lib = None
        self._load_library()
    
    def _load_library(self):
        """Load the C++ shared library"""
        try:
            # Try different possible locations
            possible_paths = [
                os.path.join(os.path.dirname(__file__), 'kociemba_solver.so'),
                os.path.join(os.path.dirname(__file__), '..', 'kociemba_solver.so'),
                'kociemba_solver.so'
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    self.lib = ctypes.CDLL(path)
                    break
            
            if self.lib is None:
                raise FileNotFoundError("Could not find kociemba_solver.so")
            
            # Set up function signatures
            self.lib.kociemba_solve.argtypes = [ctypes.c_char_p]
            self.lib.kociemba_solve.restype = ctypes.c_char_p
            
            self.lib.kociemba_generate_scramble.argtypes = []
            self.lib.kociemba_generate_scramble.restype = ctypes.c_char_p
            
            self.lib.kociemba_is_valid_cube.argtypes = [ctypes.c_char_p]
            self.lib.kociemba_is_valid_cube.restype = ctypes.c_int
            
            self.lib.scramble_to_cube_string.argtypes = [ctypes.c_char_p]
            self.lib.scramble_to_cube_string.restype = ctypes.c_char_p
            
        except Exception as e:
            print(f"Warning: Could not load Kociemba C++ library: {e}")
            print("Falling back to simple inverse-move solver")
            self.lib = None
    
    def solve(self, cube_string: str) -> str:
        """Solve a cube using Kociemba's two-phase algorithm"""
        if self.lib:
            try:
                result = self.lib.kociemba_solve(cube_string.encode('utf-8'))
                if result:
                    return result.decode('utf-8')
                return ""
            except Exception as e:
                print(f"Kociemba solver error: {e}")
                return self._fallback_solve_from_cube_string(cube_string)
        else:
            return self._fallback_solve_from_cube_string(cube_string)
    
    def solve_scramble(self, scramble: str) -> str:
        """Solve a scramble sequence directly"""
        if self.lib:
            try:
                # Pass scramble directly to the C++ solver
                result = self.lib.kociemba_solve(scramble.encode('utf-8'))
                if result:
                    return result.decode('utf-8')
                return ""
            except Exception as e:
                print(f"Kociemba solver error: {e}")
                return self._fallback_solve_scramble(scramble)
        else:
            return self._fallback_solve_scramble(scramble)
    
    def generate_scramble(self) -> str:
        """Generate a random scramble"""
        if self.lib:
            try:
                result = self.lib.kociemba_generate_scramble()
                if result:
                    return result.decode('utf-8')
                return ""
            except Exception as e:
                print(f"Kociemba scramble generation error: {e}")
                return self._fallback_generate_scramble()
        else:
            return self._fallback_generate_scramble()
    
    def is_valid_cube(self, cube_string: str) -> bool:
        """Check if a cube string is valid"""
        if self.lib:
            try:
                result = self.lib.kociemba_is_valid_cube(cube_string.encode('utf-8'))
                return bool(result)
            except Exception as e:
                print(f"Kociemba validation error: {e}")
                return self._fallback_is_valid_cube(cube_string)
        else:
            return self._fallback_is_valid_cube(cube_string)
    
    def scramble_to_cube_string(self, scramble: str) -> str:
        """Convert a scramble sequence to cube string representation"""
        if self.lib:
            try:
                result = self.lib.scramble_to_cube_string(scramble.encode('utf-8'))
                if result:
                    return result.decode('utf-8')
                return "000000000111111111222222222333333333444444444555555555"
            except Exception as e:
                print(f"Scramble conversion error: {e}")
                return "000000000111111111222222222333333333444444444555555555"  # Solved cube
        else:
            return "000000000111111111222222222333333333444444444555555555"  # Solved cube
    
    def _fallback_solve_scramble(self, scramble: str) -> str:
        """Fallback: simple inverse-move solver for scrambles"""
        move_inverses = {
            'U': "U'", "U'": 'U', 'U2': 'U2',
            'R': "R'", "R'": 'R', 'R2': 'R2',
            'F': "F'", "F'": 'F', 'F2': 'F2',
            'D': "D'", "D'": 'D', 'D2': 'D2',
            'L': "L'", "L'": 'L', 'L2': 'L2',
            'B': "B'", "B'": 'B', 'B2': 'B2'
        }
        
        try:
            moves = scramble.split()
            solution_moves = []
            
            for move in reversed(moves):
                if move in move_inverses:
                    solution_moves.append(move_inverses[move])
                else:
                    solution_moves.append(move)
            
            return ' '.join(solution_moves)
        except Exception as e:
            return f"Error: {str(e)}"
    
    def _fallback_solve_from_cube_string(self, cube_string: str) -> str:
        """Fallback: cannot solve arbitrary cube strings without Kociemba"""
        return "Error: Cannot solve arbitrary cube states without Kociemba algorithm"
    
    def _fallback_generate_scramble(self) -> str:
        """Fallback scramble generation"""
        import random
        moves = ['U', "U'", 'U2', 'R', "R'", 'R2', 'F', "F'", 'F2', 
                'D', "D'", 'D2', 'L', "L'", 'L2', 'B', "B'", 'B2']
        
        scramble_moves = []
        last_face = -1
        
        for _ in range(25):
            # Avoid consecutive moves on same face
            available_moves = [m for m in moves if m[0] != (chr(ord('U') + last_face) if last_face >= 0 else '')]
            move = random.choice(available_moves)
            scramble_moves.append(move)
            last_face = ord(move[0]) - ord('U')
        
        return ' '.join(scramble_moves)
    
    def _fallback_is_valid_cube(self, cube_string: str) -> bool:
        """Fallback validation for cube strings"""
        if len(cube_string) != 54:
            return False
        
        # Check that each color (0-5) appears exactly 9 times
        color_counts = [0] * 6
        for char in cube_string:
            if not char.isdigit() or int(char) < 0 or int(char) > 5:
                return False
            color_counts[int(char)] += 1
        
        return all(count == 9 for count in color_counts)

# Global solver instance
_kociemba_solver = None

def get_kociemba_solver():
    """Get or create the global Kociemba solver instance"""
    global _kociemba_solver
    if _kociemba_solver is None:
        _kociemba_solver = KociembaSolverWrapper()
    return _kociemba_solver

# Convenience functions
def solve_cube(cube_string: str) -> str:
    """Solve a cube using Kociemba's algorithm"""
    return get_kociemba_solver().solve(cube_string)

def solve_scramble(scramble: str) -> str:
    """Solve a scramble sequence"""
    return get_kociemba_solver().solve_scramble(scramble)

def generate_scramble() -> str:
    """Generate a random scramble"""
    return get_kociemba_solver().generate_scramble()

def is_valid_cube(cube_string: str) -> bool:
    """Check if a cube string is valid"""
    return get_kociemba_solver().is_valid_cube(cube_string)

def scramble_to_cube_string(scramble: str) -> str:
    """Convert scramble to cube string"""
    return get_kociemba_solver().scramble_to_cube_string(scramble)

if __name__ == "__main__":
    print("Testing Kociemba Solver Wrapper...")
    
    solver = get_kociemba_solver()
    print(f"Library loaded: {'Yes' if solver.lib else 'No'}")
    
    # Test 1: Generate scramble
    scramble = generate_scramble()
    print(f"Generated scramble: {scramble}")
    
    # Test 2: Test scramble solving (this should work)
    solution = solve_scramble(scramble)
    print(f"Solution for scramble '{scramble}': {solution}")
    
    # Test 3: Test simple scramble
    test_scramble = "R U R' U'"
    test_solution = solve_scramble(test_scramble)
    print(f"Scramble '{test_scramble}' solution: {test_solution}")
    
    # Test 4: Test cube string validation
    cube_string = scramble_to_cube_string(scramble)
    print(f"Cube string: {cube_string}")
    is_valid = is_valid_cube(cube_string)
    print(f"Cube string is valid: {is_valid}")
    
    print("Kociemba wrapper test completed!")
