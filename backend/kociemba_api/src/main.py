#!/usr/bin/env python3
"""
Enhanced Rubik's Cube Solver API
High-performance Flask backend with custom Kociemba algorithm integration

This Flask application provides a comprehensive REST API for Rubik's Cube solving
using our in-house developed Kociemba two-phase algorithm implementation.

Features:
- Advanced session management with state tracking
- Custom high-performance C++ Kociemba solver backend
- Cross-platform compatibility (macOS, Linux, Windows)
- Comprehensive error handling and logging
- Optimized for localhost deployment

Author: Internal Development Team
Version: 2.0
"""

from flask import Flask, request, jsonify
from flask_cors import CORS
import time
import traceback
import os
import sys

# Add the current directory to Python path for imports
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, current_dir)

try:
    from kociemba_custom_solver import (
        get_custom_solver, solve_cube, solve_scramble, generate_scramble,
        is_valid_cube, clear_session, get_session_info
    )
    CUSTOM_SOLVER_AVAILABLE = True
    print("Custom Kociemba solver loaded successfully")
except ImportError as e:
    print(f"Warning: Could not import custom Kociemba solver: {e}")
    CUSTOM_SOLVER_AVAILABLE = False

# Initialize Flask application
app = Flask(__name__)

# Configure CORS for localhost development
CORS(app, origins=[
    "http://localhost:3000",    # React development server
    "http://localhost:5173",    # Vite development server
    "http://localhost:8080",    # Alternative development port
    "http://127.0.0.1:3000",    # IPv4 localhost variants
    "http://127.0.0.1:5173",
    "http://127.0.0.1:8080",
    "http://localhost:3001",    # Additional common ports
    "http://localhost:4000",
    "http://localhost:8000"
])

# Application configuration
app.config['JSON_SORT_KEYS'] = False

def log_request_info(endpoint_name: str, request_data: dict = None):
    """Log request information for debugging"""
    print(f"[{time.strftime('%Y-%m-%d %H:%M:%S')}] {endpoint_name} - Request received")
    if request_data:
        print(f"Request data: {request_data}")

def create_error_response(message: str, status_code: int = 400):
    """Create standardized error response"""
    return jsonify({
        'success': False,
        'error': message,
        'timestamp': time.time()
    }), status_code

def create_success_response(data: dict):
    """Create standardized success response"""
    response_data = {
        'success': True,
        'timestamp': time.time()
    }
    response_data.update(data)
    return jsonify(response_data)

@app.route('/', methods=['GET'])
def health_check():
    """
    Health check endpoint providing system status and capabilities
    """
    try:
        return jsonify({
            'status': 'ok',
            'message': 'Enhanced Kociemba Rubik\'s Cube Solver API',
            'version': '2.0',
            'custom_solver_available': CUSTOM_SOLVER_AVAILABLE,
            'features': [
                'Advanced session management',
                'High-performance C++ solver backend',
                'Cross-platform compatibility',
                'Comprehensive state tracking',
                'Localhost deployment optimized'
            ],
            'endpoints': {
                'health': 'GET /',
                'scramble': 'GET /api/scramble',
                'solve_state': 'POST /api/solve',
                'solve_scramble': 'POST /api/solve_scramble',
                'validate': 'POST /api/validate',
                'session_clear': 'POST /api/session/clear',
                'session_info': 'GET /api/session/info'
            },
            'timestamp': time.time()
        })
    except Exception as e:
        return create_error_response(f"Health check failed: {str(e)}", 500)

@app.route('/api/scramble', methods=['GET'])
def generate_scramble_endpoint():
    """
    Generate a random scramble sequence with corresponding cube state
    
    Returns:
        JSON response with scramble sequence and cube state
    """
    log_request_info("Generate Scramble")
    
    try:
        if not CUSTOM_SOLVER_AVAILABLE:
            return create_error_response("Custom solver not available")
        
        # Get scramble length from query parameters (default: 25)
        scramble_length = request.args.get('length', 25, type=int)
        if scramble_length < 10 or scramble_length > 50:
            scramble_length = 25
        
        # Generate scramble using custom solver
        scramble, cube_state = generate_scramble()
        
        return create_success_response({
            'scramble': scramble,
            'cube_state': cube_state,
            'scramble_length': len(scramble.split())
        })
        
    except Exception as e:
        print(f"Scramble generation error: {traceback.format_exc()}")
        return create_error_response(f"Failed to generate scramble: {str(e)}", 500)

@app.route('/api/solve', methods=['POST'])
def solve_cube_state_endpoint():
    """
    Solve a cube from its current state representation
    
    Expected JSON payload:
    {
        "cube_state": "54-character string representing cube state",
        "session_id": "optional session identifier"
    }
    
    Returns:
        JSON response with solution moves
    """
    try:
        data = request.get_json()
        if not data:
            return create_error_response("No JSON data provided")
        
        log_request_info("Solve Cube State", data)
        
        if not CUSTOM_SOLVER_AVAILABLE:
            return create_error_response("Custom solver not available")
        
        # Extract cube state and session ID
        cube_state = data.get('cube_state', '').strip()
        session_id = data.get('session_id', 'default')
        
        if not cube_state:
            return create_error_response("Missing cube_state parameter")
        
        # Validate cube state format
        if not is_valid_cube(cube_state):
            return create_error_response("Invalid cube state format")
        
        # Solve the cube
        start_time = time.time()
        solution = solve_cube(cube_state, session_id)
        solve_time = (time.time() - start_time) * 1000  # Convert to milliseconds
        
        if solution.startswith("Error"):
            return create_error_response(solution)
        
        return create_success_response({
            'solution': solution,
            'session_id': session_id,
            'solve_time_ms': round(solve_time, 2),
            'move_count': len(solution.split()) if solution else 0
        })
        
    except Exception as e:
        print(f"Cube solving error: {traceback.format_exc()}")
        return create_error_response(f"Failed to solve cube: {str(e)}", 500)

@app.route('/api/solve_scramble', methods=['POST'])
def solve_scramble_endpoint():
    """
    Solve a scramble sequence using advanced Kociemba algorithm
    
    Expected JSON payload:
    {
        "scramble": "space-separated sequence of moves",
        "session_id": "optional session identifier"
    }
    
    Returns:
        JSON response with solution moves
    """
    try:
        data = request.get_json()
        if not data:
            return create_error_response("No JSON data provided")
        
        log_request_info("Solve Scramble", data)
        
        if not CUSTOM_SOLVER_AVAILABLE:
            return create_error_response("Custom solver not available")
        
        # Extract scramble and session ID
        scramble = data.get('scramble', '').strip()
        session_id = data.get('session_id', 'default')
        
        if not scramble:
            return create_error_response("Missing scramble parameter")
        
        # Solve the scramble
        start_time = time.time()
        solution = solve_scramble(scramble, session_id)
        solve_time = (time.time() - start_time) * 1000  # Convert to milliseconds
        
        if solution.startswith("Error"):
            return create_error_response(solution)
        
        return create_success_response({
            'solution': solution,
            'scramble': scramble,
            'session_id': session_id,
            'solve_time_ms': round(solve_time, 2),
            'move_count': len(solution.split()) if solution else 0
        })
        
    except Exception as e:
        print(f"Scramble solving error: {traceback.format_exc()}")
        return create_error_response(f"Failed to solve scramble: {str(e)}", 500)

@app.route('/api/validate', methods=['POST'])
def validate_cube_endpoint():
    """
    Validate a cube state for correctness and solvability
    
    Expected JSON payload:
    {
        "cube_state": "54-character string representing cube state"
    }
    
    Returns:
        JSON response with validation result
    """
    try:
        data = request.get_json()
        if not data:
            return create_error_response("No JSON data provided")
        
        log_request_info("Validate Cube", data)
        
        cube_state = data.get('cube_state', '').strip()
        if not cube_state:
            return create_error_response("Missing cube_state parameter")
        
        # Validate the cube state
        is_valid = is_valid_cube(cube_state) if CUSTOM_SOLVER_AVAILABLE else False
        
        return create_success_response({
            'is_valid': is_valid,
            'cube_state': cube_state,
            'validation_details': {
                'length_check': len(cube_state) == 54,
                'format_check': all(c in 'URFDLB' for c in cube_state.upper()),
                'solver_available': CUSTOM_SOLVER_AVAILABLE
            }
        })
        
    except Exception as e:
        print(f"Validation error: {traceback.format_exc()}")
        return create_error_response(f"Failed to validate cube: {str(e)}", 500)

@app.route('/api/session/clear', methods=['POST'])
def clear_session_endpoint():
    """
    Clear a session's state and reset to solved cube
    
    Expected JSON payload:
    {
        "session_id": "optional session identifier"
    }
    
    Returns:
        JSON response confirming session clear
    """
    try:
        data = request.get_json() or {}
        session_id = data.get('session_id', 'default')
        
        log_request_info("Clear Session", {'session_id': session_id})
        
        if not CUSTOM_SOLVER_AVAILABLE:
            return create_error_response("Custom solver not available")
        
        # Clear the session
        clear_session(session_id)
        
        return create_success_response({
            'message': 'Session cleared successfully',
            'session_id': session_id
        })
        
    except Exception as e:
        print(f"Session clear error: {traceback.format_exc()}")
        return create_error_response(f"Failed to clear session: {str(e)}", 500)

@app.route('/api/session/info', methods=['GET'])
def get_session_info_endpoint():
    """
    Get comprehensive information about a session
    
    Query parameters:
        session_id: Session identifier (default: 'default')
    
    Returns:
        JSON response with session information
    """
    try:
        session_id = request.args.get('session_id', 'default')
        
        log_request_info("Get Session Info", {'session_id': session_id})
        
        if not CUSTOM_SOLVER_AVAILABLE:
            return create_error_response("Custom solver not available")
        
        # Get session information
        session_info = get_session_info(session_id)
        
        return create_success_response({
            'session_info': session_info
        })
        
    except Exception as e:
        print(f"Session info error: {traceback.format_exc()}")
        return create_error_response(f"Failed to get session info: {str(e)}", 500)

@app.errorhandler(404)
def not_found_error(error):
    """Handle 404 errors"""
    return create_error_response("Endpoint not found", 404)

@app.errorhandler(405)
def method_not_allowed_error(error):
    """Handle 405 errors"""
    return create_error_response("Method not allowed", 405)

@app.errorhandler(500)
def internal_error(error):
    """Handle 500 errors"""
    return create_error_response("Internal server error", 500)

if __name__ == '__main__':
    print("Starting Enhanced Kociemba Rubik's Cube Solver API...")
    print(f"Custom solver available: {CUSTOM_SOLVER_AVAILABLE}")
    print("Server optimized for localhost deployment")
    print("Available endpoints:")
    print("  GET  /                    - Health check and API information")
    print("  GET  /api/scramble        - Generate random scramble")
    print("  POST /api/solve           - Solve cube from state")
    print("  POST /api/solve_scramble  - Solve scramble sequence")
    print("  POST /api/validate        - Validate cube state")
    print("  POST /api/session/clear   - Clear session state")
    print("  GET  /api/session/info    - Get session information")
    print()
    
    # Run the Flask development server
    app.run(
        debug=True,
        port=5001,
        host='0.0.0.0'  # Allow connections from any interface
    )

