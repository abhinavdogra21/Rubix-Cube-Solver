#!/usr/bin/env python3
"""
Python wrapper for the Kociemba two-phase algorithm C++ solver
"""

import ctypes
import os
import sys

# Add RTLD_GLOBAL for proper symbol resolution
if sys.platform.startswith("linux"):
    # Equivalent to RTLD_GLOBAL | RTLD_LAZY
    _RTLD_GLOBAL = 0x00100 | 0x00001
else:
    _RTLD_GLOBAL = 0 # Not applicable or not needed on other platforms

class KociembaSolverWrapper:
    def __init__(self):
        self.lib = None
        self._load_library()
    
    def _load_library(self):
        """Load the C++ shared library"""
        try:
            # Try different possible locations
            # Prioritize the location where setup.py places it
            possible_paths = [
                os.path.join(os.path.dirname(__file__), 'kociemba_solver.so'),
                os.path.join(os.path.dirname(__file__), '..', 'kociemba_solver.so'),
                # Fallback to current directory if not found in expected paths
                'kociemba_solver.so'
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    # Load with RTLD_GLOBAL to make symbols available to other libraries
                    self.lib = ctypes.CDLL(path, mode=_RTLD_GLOBAL)
                    print(f"Successfully loaded C++ library from: {path}")
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
            print("In-house solver not available")
            self.lib = None
    
    def solve(self, cube_string: str) -> str:
        """Solve a cube using in-house Kociemba's two-phase algorithm."""
        
        # Use only native library
        if self.lib:
            try:
                result = self.lib.kociemba_solve(cube_string.encode('utf-8'))
                if result:
                    solved = result.decode('utf-8').strip()
                    if solved:
                        return solved
                    else:
                        return "Error: No solution found"
                else:
                    return "Error: Solver returned empty result"
            except Exception as e:
                return f"Error: Native kociemba solver error - {str(e)}"
        else:
            return "Error: In-house Kociemba solver not available. Please ensure kociemba_solver.so is built and accessible."
    
    def solve_scramble(self, scramble: str) -> str:
        """Solve a scramble sequence directly using in-house solver"""
        if self.lib:
            try:
                result = self.lib.kociemba_solve(scramble.encode('utf-8'))
                if result:
                    solved = result.decode('utf-8').strip()
                    if solved:
                        return solved
                    else:
                        return "Error: No solution found"
                else:
                    return "Error: Solver returned empty result"
            except Exception as e:
                return f"Error: Kociemba solver error - {str(e)}"
        else:
            return "Error: In-house Kociemba solver not available. Please ensure kociemba_solver.so is built and accessible."
    
    def generate_scramble(self) -> str:
        """Generate a random scramble using in-house solver"""
        if self.lib:
            try:
                result = self.lib.kociemba_generate_scramble()
                if result:
                    return result.decode('utf-8')
                else:
                    return self._fallback_generate_scramble()
            except Exception as e:
                print(f"Kociemba scramble generation error: {e}")
                return self._fallback_generate_scramble()
        else:
            return self._fallback_generate_scramble()
    
    def is_valid_cube(self, cube_string: str) -> bool:
        """Check if a cube string is valid using in-house solver"""
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
        """Convert a scramble sequence to cube string representation using in-house solver"""
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
    
    def _fallback_generate_scramble(self) -> str:
        """Fallback scramble generation when in-house solver is not available"""
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
        """Basic fallback validation when in-house solver is not available"""
        if len(cube_string) != 54:
            return False
        
        if set(cube_string) <= set('012345'):
            # Count each digit - should be exactly 9 of each
            color_counts = [0] * 6
            for char in cube_string:
                if not char.isdigit() or int(char) < 0 or int(char) > 5:
                    return False
                color_counts[int(char)] += 1
            return all(count == 9 for count in color_counts)
        else:
            # URFDLB format
            if not set(cube_string.upper()) <= set('URFDLB'):
                return False
            color_counts = {}
            for c in cube_string.upper():
                color_counts[c] = color_counts.get(c, 0) + 1
            return all(count == 9 for count in color_counts.values()) and len(color_counts) == 6

# Global solver instance
_kociemba_solver = None

def get_kociemba_solver():
    """Get or create the global Kociemba solver instance"""
    global _kociemba_solver
    if _kociemba_solver is None:
        _kociemba_solver = KociembaSolverWrapper()
    return _kociememba_solver

# Convenience functions
def solve_cube(cube_string: str) -> str:
    """Solve a cube using in-house Kociemba's algorithm"""
    return get_kociemba_solver().solve(cube_string)

def solve_scramble(scramble: str) -> str:
    """Solve a scramble sequence using in-house solver"""
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
    print("Testing In-House Kociemba Solver Wrapper...")
    
    solver = get_kociemba_solver()
    print(f"In-house library loaded: {'Yes' if solver.lib else 'No'}")
    
    # Test 1: Generate scramble
    scramble = generate_scramble()
    print(f"Generated scramble: {scramble}")
    
    # Test 2: Test scramble solving
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
    
    print("In-house Kociemba wrapper test completed!")


