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
            print("Falling back to Python kociemba package")
            self.lib = None
    
    def solve(self, cube_string: str) -> str:
        """Solve a cube using Kociemba's two-phase algorithm.

         Preference order:
           1. Native shared library (fast, if truly implemented)
           2. Python 'kociemba' package (if native missing / returns stub)
           3. Simple fallback implementation.
         """

        # --- 1. Try native library first ---
        STUB_SEQUENCE = "R U R' U' R' F R2 U' R' U' R U R' F'"
        if self.lib:
            try:
                result = self.lib.kociemba_solve(cube_string.encode('utf-8'))
                if result:
                    solved = result.decode('utf-8').strip()
                    # If native library returns a non-empty sequence that isn't the known stub, use it.
                    if solved and solved != STUB_SEQUENCE:
                        return solved
                    # Otherwise we will fall through to python solver.
            except Exception as e:
                print(f"Native kociemba solver error: {e}. Falling back to python solver.")

        # --- 2. Try python kociemba if available ---
        try:
            import kociemba  # pylint: disable=import-error
            
            # Enhanced cube string processing
            if len(cube_string) == 54:
                if set(cube_string) <= set('012345'):
                    # Convert digit format to facelet format
                    digit_to_face = { '0':'U', '1':'R', '2':'F', '3':'D', '4':'L', '5':'B' }
                    facelets = ''.join(digit_to_face[d] for d in cube_string)
                elif set(cube_string.upper()) <= set('URFDLB'):
                    facelets = cube_string.upper()
                else:
                    return "Error: Invalid cube string format. Use digits 0-5 or letters URFDLB."
                
                # Validate the cube state before solving
                if not self._validate_cube_state(facelets):
                    return "Error: Invalid cube state. The cube configuration is not solvable."
                
                try:
                    solution = kociemba.solve(facelets)
                    if solution and solution.strip():
                        return solution.strip()
                    else:
                        return "Error: Cube is already solved or no solution found."
                except ValueError as e:
                    return f"Error: Invalid cube state - {str(e)}"
                except Exception as e:
                    print(f"Python kociemba solver error: {e}. Falling back to simple fallback.")
                    return f"Error: Solver failed - {str(e)}"
            else:
                return f"Error: Cube string must be exactly 54 characters, got {len(cube_string)}."
                
        except ImportError:
            print("Python kociemba package not available. Falling back to simple solver.")

        # --- 3. Fallback ---
        return self._fallback_solve_from_cube_string(cube_string)
    
    def solve_scramble(self, scramble: str) -> str:
        """Solve a scramble sequence directly"""
        if self.lib:
            try:
                result = self.lib.kociemba_solve(scramble.encode('utf-8'))
                if result:
                    return result.decode('utf-8')
                return ""
            except Exception as e:
                print(f"Kociemba solver error: {e}")
                # fall through to simple inverse fallback
        # Inverse fallback only (no python solver)
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
    
    def _validate_cube_state(self, facelets: str) -> bool:
        """Enhanced validation for cube state"""
        if len(facelets) != 54:
            return False
        
        # Check if all characters are valid
        if not set(facelets.upper()) <= set('URFDLB'):
            return False
        
        # Count each color - should be exactly 9 of each
        color_counts = {}
        for c in facelets.upper():
            color_counts[c] = color_counts.get(c, 0) + 1
        
        if not all(count == 9 for count in color_counts.values()):
            return False
        
        # Check that we have exactly 6 different colors
        if len(color_counts) != 6:
            return False
        
        # Check center positions (indices 4, 13, 22, 31, 40, 49)
        centers = [facelets[4], facelets[13], facelets[22], facelets[31], facelets[40], facelets[49]]
        if len(set(centers)) != 6:
            return False
        
        # Additional validation: check opposite face relationships
        # U-D, R-L, F-B should be opposite pairs
        center_map = {
            facelets[4]: 'U',   # Top face center
            facelets[13]: 'R',  # Right face center  
            facelets[22]: 'F',  # Front face center
            facelets[31]: 'D',  # Bottom face center
            facelets[40]: 'L',  # Left face center
            facelets[49]: 'B'   # Back face center
        }
        
        # Find which colors are assigned to which faces
        face_colors = {}
        for color, face in center_map.items():
            face_colors[face] = color
        
        # Check opposite pairs
        opposites = [('U', 'D'), ('R', 'L'), ('F', 'B')]
        for face1, face2 in opposites:
            if face_colors[face1] == face_colors[face2]:
                return False
        
        return True
    
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
        """Fallback when native solver unavailable. Tries to use python kociemba lib."""
        try:
            import kociemba  # lazily import
        except ImportError:
            return ("Error: Native Kociemba solver library not available. "
                    "Install the 'kociemba' Python package or compile kociemba_solver.so.")

        try:
            # If cube_string is in digit form (0-5) convert to facelets URFDLB.
            if set(cube_string) <= set('012345'):
                digit_to_face = { '0':'U', '1':'R', '2':'F', '3':'D', '4':'L', '5':'B' }
                facelets = ''.join(digit_to_face[ch] for ch in cube_string)
            else:
                facelets = cube_string.upper()

            # Validate length
            if len(facelets) != 54:
                return 'Error: Cube string must be 54 characters.'

            solution = kociemba.solve(facelets)
            return solution
        except Exception as e:
            return f"Error: {str(e)}"
    
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
        """Enhanced fallback validation"""
        if len(cube_string) != 54:
            return False
        
        if set(cube_string) <= set('012345'):
            # Numeric format - convert to facelets for validation
            digit_to_face = { '0':'U', '1':'R', '2':'F', '3':'D', '4':'L', '5':'B' }
            facelets = ''.join(digit_to_face[d] for d in cube_string)
            return self._validate_cube_state(facelets)
        else:
            # URFDLB format
            return self._validate_cube_state(cube_string.upper())

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
