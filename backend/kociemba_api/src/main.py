#!/usr/bin/env python3
"""
Flask API for Kociemba Rubik's Cube Solver
"""

import os
import sys
from flask import Flask, request, jsonify
from flask_cors import CORS

# Add the parent directory to Python path to import our solver
sys.path.append(os.path.join(os.path.dirname(__file__), '..', 'src'))

try:
    from kociemba_wrapper import solve_cube, solve_scramble, generate_scramble, is_valid_cube, scramble_to_cube_string
    KOCIEMBA_AVAILABLE = True
except ImportError as e:
    print(f"Warning: Could not import Kociemba wrapper: {e}")
    KOCIEMBA_AVAILABLE = False

app = Flask(__name__)
CORS(app, origins=[
    "http://localhost:3000",
    "https://rubix-cube-solver.vercel.app",
    "https://rubix-cube-solver-q2ri1oubi-abhinav-dogras-projects.vercel.app"
])  # Allow local dev and Vercel domains

@app.route('/')
def index():
    """Health check endpoint"""
    return jsonify({
        'status': 'ok',
        'message': 'Kociemba Rubik\'s Cube Solver API',
        'kociemba_available': KOCIEMBA_AVAILABLE
    })

@app.route('/api/scramble', methods=['GET'])
def api_generate_scramble():
    """Generate a random scramble"""
    try:
        if KOCIEMBA_AVAILABLE:
            scramble = generate_scramble()
            cube_state = scramble_to_cube_string(scramble)
            return jsonify({
                'success': True,
                'scramble': scramble,
                'cube_state': cube_state
            })
        else:
            # Fallback scramble generation
            import random
            moves = ['U', "U'", 'U2', 'R', "R'", 'R2', 'F', "F'", 'F2', 
                    'D', "D'", 'D2', 'L', "L'", 'L2', 'B', "B'", 'B2']
            scramble = ' '.join(random.choices(moves, k=25))
            return jsonify({
                'success': True,
                'scramble': scramble,
                'cube_state': '000000000111111111222222222333333333444444444555555555'
            })
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

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
        
        # Enhanced validation and debugging
        print(f"Received cube state: {cube_state}")
        print(f"Cube state length: {len(cube_state)}")
        print(f"Cube state characters: {set(cube_state)}")
        
        if KOCIEMBA_AVAILABLE:
            # Validate cube state first
            if not is_valid_cube(cube_state):
                return jsonify({
                    'success': False,
                    'error': 'Invalid cube state: The cube configuration is not valid or solvable'
                }), 400
            
            solution = solve_cube(cube_state)
            
            # Check if solution contains an error message
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

@app.route('/api/solve_scramble', methods=['POST'])
def api_solve_scramble():
    """Solve a scramble sequence"""
    try:
        data = request.get_json()
        if not data or 'scramble' not in data:
            return jsonify({
                'success': False,
                'error': 'Missing scramble parameter'
            }), 400
        
        scramble = data['scramble']
        
        if KOCIEMBA_AVAILABLE:
            solution = solve_scramble(scramble)
            return jsonify({
                'success': True,
                'solution': solution
            })
        else:
            # Fallback: simple inverse-move solver
            move_inverses = {
                'U': "U'", "U'": 'U', 'U2': 'U2',
                'R': "R'", "R'": 'R', 'R2': 'R2',
                'F': "F'", "F'": 'F', 'F2': 'F2',
                'D': "D'", "D'": 'D', 'D2': 'D2',
                'L': "L'", "L'": 'L', 'L2': 'L2',
                'B': "B'", "B'": 'B', 'B2': 'B2'
            }
            
            moves = scramble.split()
            solution_moves = []
            
            for move in reversed(moves):
                if move in move_inverses:
                    solution_moves.append(move_inverses[move])
                else:
                    solution_moves.append(move)
            
            solution = ' '.join(solution_moves)
            return jsonify({
                'success': True,
                'solution': solution
            })
            
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

@app.route('/api/validate', methods=['POST'])
def api_validate_cube():
    """Validate a cube state"""
    try:
        data = request.get_json()
        if not data or 'cube_state' not in data:
            return jsonify({
                'success': False,
                'error': 'Missing cube_state parameter'
            }), 400
        
        cube_state = data['cube_state']
        
        if KOCIEMBA_AVAILABLE:
            is_valid = is_valid_cube(cube_state)
            return jsonify({
                'success': True,
                'is_valid': is_valid
            })
        else:
            # Fallback validation
            if len(cube_state) != 54:
                is_valid = False
            else:
                color_counts = [0] * 6
                for char in cube_state:
                    if not char.isdigit() or int(char) < 0 or int(char) > 5:
                        is_valid = False
                        break
                    color_counts[int(char)] += 1
                else:
                    is_valid = all(count == 9 for count in color_counts)
            
            return jsonify({
                'success': True,
                'is_valid': is_valid
            })
            
    except Exception as e:
        return jsonify({
            'success': False,
            'error': str(e)
        }), 500

@app.route('/solve', methods=['POST'])
def solve():
    try:
        data = request.get_json()
        if not data or 'state' not in data:
            return jsonify({'error': 'Missing cube state'}), 400
        
        cube_state = data['state']
        solution = solve_cube(cube_state)
        return jsonify({'solution': solution})
    
    except ValueError as e:
        return jsonify({'error': str(e)}), 400
    except Exception as e:
        return jsonify({'error': 'Internal server error: ' + str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True, port=5001)

