# https://github.com/abhinavdogra21/Rubix-Cube-Solver
#!/usr/bin/env python3
"""
Flask API for Kociemba Rubik's Cube Solver
"""

import os
import sys
import random
from flask import Flask, request, jsonify
from flask_cors import CORS

# Add the src directory to Python path to import our solver
sys.path.append(os.path.dirname(__file__))

try:
    from kociemba_solver import solve
except ImportError as e:
    print(f"Warning: Could not import Kociemba solver: {e}")
    KOCIEMBA_AVAILABLE = False
else:
    KOCIEMBA_AVAILABLE = True

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

@app.route('/')
def index():
    """Health check endpoint"""
    return jsonify({
        'status': 'ok',
        'message': 'Kociemba Rubik\'s Cube Solver API',
        'kociemba_available': KOCIEMBA_AVAILABLE
    })

@app.route('/api/solve', methods=['POST'])
def api_solve_cube():
    """Solve a cube from cube state string"""
    try:
        data = request.get_json()
        if not data or 'cube_state' not in data:
            return jsonify({
                'success': False,
                'error': 'Missing cube_state parameter'
            }), 400
        cube_state = data['cube_state']
        print(f"Received cube state: {cube_state}")
        print(f"Cube state length: {len(cube_state)}")
        print(f"Cube state characters: {set(cube_state)}")
        # Convert digit cube state (0-5) to letter format for solver
        digit_to_letter = ['U', 'R', 'F', 'D', 'L', 'B']
        if all(c in '012345' for c in cube_state) and len(cube_state) == 54:
            cube_state_letters = ''.join(digit_to_letter[int(c)] for c in cube_state)
        else:
            cube_state_letters = cube_state
        if KOCIEMBA_AVAILABLE:
            solution = solve(cube_state_letters)
            if solution.startswith('Error:'):
                return jsonify({
                    'success': False,
                    'error': solution
                }), 400
            return jsonify({
                'success': True,
                'solution': solution
            })
        else:
            return jsonify({
                'success': False,
                'error': 'Kociemba solver not available'
            }), 503
    except Exception as e:
        print(f"Solve error: {str(e)}")
        return jsonify({
            'success': False,
            'error': f'Internal server error: {str(e)}'
        }), 500

# Helper: apply scramble to solved cube state
# Faces: U=0, R=1, F=2, D=3, L=4, B=5
SOLVED_CUBE = '000000000111111111222222222333333333444444444555555555'
MOVE_MAP = {
    "U": [0,1,2,3,4,5,6,7,8], "R": [9,10,11,12,13,14,15,16,17], "F": [18,19,20,21,22,23,24,25,26],
    "D": [27,28,29,30,31,32,33,34,35], "L": [36,37,38,39,40,41,42,43,44], "B": [45,46,47,48,49,50,51,52,53]
}
# This is a placeholder. For a real scramble, you need a full cube simulator.
def apply_scramble(cube_state, scramble_moves):
    # For now, just return the solved state (real implementation would permute stickers)
    return cube_state

@app.route('/api/scramble', methods=['GET'])
def api_generate_scramble():
    """Generate a random scramble and scrambled cube state"""
    moves = ['U', "U'", 'U2', 'R', "R'", 'R2', 'F', "F'", 'F2', 'D', "D'", 'D2', 'L', "L'", 'L2', 'B', "B'", 'B2']
    scramble = ' '.join(random.choices(moves, k=25))
    scramble_moves = scramble.split()
    scrambled_cube_state = apply_scramble(SOLVED_CUBE, scramble_moves)
    return jsonify({
        'success': True,
        'scramble': scramble,
        'cube_state': scrambled_cube_state
    })

if __name__ == '__main__':
    app.run(debug=True, port=5001)

