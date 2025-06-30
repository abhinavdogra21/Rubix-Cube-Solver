"""
Python wrapper for the custom C++ cube solver
"""

import ctypes
import os
import sys

class CustomCubeSolverWrapper:
    def __init__(self):
        self.lib = None
        self._load_library()
    
    def _load_library(self):
        """Load the C++ shared library"""
        try:
            # Try different possible locations
            possible_paths = [
                os.path.join(os.path.dirname(__file__), 'custom_cube_solver.so'),
                os.path.join(os.path.dirname(__file__), '..', 'custom_cube_solver.so'),
                'custom_cube_solver.so'
            ]
            
            for path in possible_paths:
                if os.path.exists(path):
                    self.lib = ctypes.CDLL(path)
                    break
            
            if self.lib is None:
                raise FileNotFoundError("Could not find custom_cube_solver.so")
            
            # Set up function signatures
            self.lib.solve_cube.argtypes = [ctypes.c_char_p]
            self.lib.solve_cube.restype = ctypes.c_char_p
            
            self.lib.generate_scramble.argtypes = []
            self.lib.generate_scramble.restype = ctypes.c_char_p
            
            self.lib.is_valid_scramble.argtypes = [ctypes.c_char_p]
            self.lib.is_valid_scramble.restype = ctypes.c_int
            
        except Exception as e:
            print(f"Warning: Could not load C++ library: {e}")
            print("Falling back to Python implementation")
            self.lib = None
    
    def solve(self, scramble: str) -> str:
        """Solve a scrambled cube"""
        if self.lib:
            try:
                result = self.lib.solve_cube(scramble.encode('utf-8'))
                return result.decode('utf-8')
            except Exception as e:
                print(f"C++ solver error: {e}")
                return self._fallback_solve(scramble)
        else:
            return self._fallback_solve(scramble)
    
    def generate_scramble(self) -> str:
        """Generate a random scramble"""
        if self.lib:
            try:
                result = self.lib.generate_scramble()
                return result.decode('utf-8')
            except Exception as e:
                print(f"C++ scramble generation error: {e}")
                return self._fallback_generate_scramble()
        else:
            return self._fallback_generate_scramble()
    
    def is_valid_scramble(self, scramble: str) -> bool:
        """Check if a scramble is valid"""
        if self.lib:
            try:
                result = self.lib.is_valid_scramble(scramble.encode('utf-8'))
                return bool(result)
            except Exception as e:
                print(f"C++ validation error: {e}")
                return self._fallback_is_valid_scramble(scramble)
        else:
            return self._fallback_is_valid_scramble(scramble)
    
    def _fallback_solve(self, scramble: str) -> str:
        """Fallback Python implementation"""
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
    
    def _fallback_generate_scramble(self) -> str:
        """Fallback scramble generation"""
        import random
        moves = ['U', "U'", 'U2', 'R', "R'", 'R2', 'F', "F'", 'F2', 
                'D', "D'", 'D2', 'L', "L'", 'L2', 'B', "B'", 'B2']
        
        scramble_moves = []
        for _ in range(20):
            scramble_moves.append(random.choice(moves))
        
        return ' '.join(scramble_moves)
    
    def _fallback_is_valid_scramble(self, scramble: str) -> bool:
        """Fallback validation"""
        valid_moves = {'U', "U'", 'U2', 'R', "R'", 'R2', 'F', "F'", 'F2',
                      'D', "D'", 'D2', 'L', "L'", 'L2', 'B', "B'", 'B2'}
        moves = scramble.split()
        return all(move in valid_moves for move in moves)

# Global solver instance
_solver = None

def get_solver():
    """Get or create the global solver instance"""
    global _solver
    if _solver is None:
        _solver = CustomCubeSolverWrapper()
    return _solver

# Convenience functions
def solve_cube(scramble: str) -> str:
    """Solve a scrambled cube"""
    return get_solver().solve(scramble)

def generate_scramble() -> str:
    """Generate a random scramble"""
    return get_solver().generate_scramble()

def is_valid_scramble(scramble: str) -> bool:
    """Check if a scramble is valid"""
    return get_solver().is_valid_scramble(scramble)

if __name__ == "__main__":
    # Test the wrapper
    solver = CustomCubeSolverWrapper()
    
    print("Testing Custom Cube Solver Wrapper...")
    
    # Test scramble generation
    scramble = solver.generate_scramble()
    print(f"Generated scramble: {scramble}")
    
    # Test validation
    is_valid = solver.is_valid_scramble(scramble)
    print(f"Scramble is valid: {is_valid}")
    
    # Test solving
    solution = solver.solve(scramble)
    print(f"Solution: {solution}")
    
    print("Wrapper test completed!")

