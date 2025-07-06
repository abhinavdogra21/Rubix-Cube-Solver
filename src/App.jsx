import React, { useState, useEffect, useRef } from 'react';
import RubiksCube from './RubiksCube';
import { CubeState } from './cubeState';
import './App.css';

const API_BASE_URL = 'http://localhost:5000'; // Ensure this matches your backend port

function App() {
  const [scrambleInput, setScrambleInput] = useState('');
  const [currentScramble, setCurrentScramble] = useState('');
  const [solution, setSolution] = useState('');
  const [cubeState, setCubeStateValue] = useState(new CubeState());
  const [isLoading, setIsLoading] = useState(false);
  const [apiStatus, setApiStatus] = useState('checking');
  const cubeRef = useRef();

  // Check API status on component mount
  useEffect(() => {
    checkApiStatus();
  }, []);

  const checkApiStatus = async () => {
    try {
      const response = await fetch(`${API_BASE_URL}/`);
      if (response.ok) {
        const data = await response.json();
        setApiStatus(data.kociemba_available ? 'available' : 'limited');
      } else {
        setApiStatus('unavailable');
      }
    } catch (error) {
      console.error('API check failed:', error);
      setApiStatus('unavailable');
    }
  };

  const generateRandomScramble = () => {
    const moves = ['R', 'L', 'U', 'D', 'F', 'B'];
    const modifiers = ['', '\'', '2'];
    const scrambleLength = 25;
    
    let scramble = [];
    let lastMove = '';
    
    for (let i = 0; i < scrambleLength; i++) {
      let move;
      do {
        move = moves[Math.floor(Math.random() * moves.length)];
      } while (move === lastMove);
      
      const modifier = modifiers[Math.floor(Math.random() * modifiers.length)];
      scramble.push(move + modifier);
      lastMove = move;
    }
    
    return scramble.join(' ');
  };

  const handleRandomScramble = async () => {
    setIsLoading(true);
    try {
      if (apiStatus === 'available') {
        // Try to get scramble from backend
        const response = await fetch(`${API_BASE_URL}/api/scramble`);
        const data = await response.json();
        
        if (data.success) {
          setScrambleInput(data.scramble);
        } else {
          // Fallback to local generation
          const scramble = generateRandomScramble();
          setScrambleInput(scramble);
        }
      } else {
        // Use local generation
        const scramble = generateRandomScramble();
        setScrambleInput(scramble);
      }
    } catch (error) {
      console.error('Scramble generation failed:', error);
      // Fallback to local generation
      const scramble = generateRandomScramble();
      setScrambleInput(scramble);
    } finally {
      setIsLoading(false);
    }
  };

  const handleApplyScramble = async () => {
    if (!scrambleInput.trim()) return;
    
    setIsLoading(true);
    setCurrentScramble(scrambleInput);
    setSolution('');
    
    try {
      // Create new cube state and apply scramble
      const newCubeState = new CubeState();
      newCubeState.applyMoves(scrambleInput);
      setCubeStateValue(newCubeState);
      
      // Update 3D visualization
      if (cubeRef.current) {
        cubeRef.current.updateCubeState(newCubeState);
        cubeRef.current.animateScramble(scrambleInput);
      }
    } catch (error) {
      console.error('Apply scramble failed:', error);
    } finally {
      setTimeout(() => setIsLoading(false), 1000);
    }
  };

  const handleSolveCube = async () => {
    if (!currentScramble) {
      alert('Please apply a scramble first!');
      return;
    }
    
    setIsLoading(true);
    
    try {
      if (apiStatus === 'available') {
        // Try backend solver first
        const response = await fetch(`${API_BASE_URL}/api/solve_scramble`, {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({ scramble: currentScramble }),
        });
        
        const data = await response.json();
        
        if (data.success && data.solution) {
          setSolution(data.solution);
          
          // Apply solution to cube
          if (cubeRef.current) {
            cubeRef.current.animateSolution(data.solution);
          }
        } else {
          throw new Error(data.error || 'Backend solver failed');
        }
      } else {
        throw new Error('Backend not available');
      }
    } catch (error) {
      console.error('Backend solve failed, using fallback:', error);
      
      // Fallback: simple inverse-move solver
      try {
        const moves = currentScramble.trim().split(/\s+/).filter(move => move.length > 0);
        const inverseMoves = moves.reverse().map(move => {
          if (move.endsWith('\'')) {
            return move.slice(0, -1);
          } else if (move.endsWith('2')) {
            return move;
          } else {
            return move + '\'';
          }
        });
        
        const solutionMoves = inverseMoves.join(' ');
        setSolution(solutionMoves);
        
        // Apply solution to cube
        if (cubeRef.current) {
          cubeRef.current.animateSolution(solutionMoves);
        }
      } catch (fallbackError) {
        console.error('Fallback solve failed:', fallbackError);
        setSolution('Error: Could not solve cube');
      }
    } finally {
      setTimeout(() => setIsLoading(false), 2000);
    }
  };

  const handleReset = () => {
    const newCubeState = new CubeState();
    setCubeStateValue(newCubeState);
    setScrambleInput('');
    setCurrentScramble('');
    setSolution('');
    
    if (cubeRef.current) {
      cubeRef.current.updateCubeState(newCubeState);
    }
  };

  const getStatusColor = () => {
    switch (apiStatus) {
      case 'available': return '#98c379'; // Greenish color from image
      case 'limited': return '#f59e0b';
      case 'unavailable': return '#ef4444';
      default: return '#6b7280';
    }
  };

  const getStatusText = () => {
    switch (apiStatus) {
      case 'available': return '🚀 Kociemba Solver Ready';
      case 'limited': return '⚡ Limited Mode (Fallback Solver)';
      case 'unavailable': return '⚠️ Backend Unavailable';
      default: return '🔍 Checking...';
    }
  };

  return (
    <div className="App">
      <header className="app-header">
        <h1>3D Interactive Rubik's Cube Solver</h1>
        <div className="status-indicator" style={{ color: getStatusColor() }}>
          {getStatusText()}
        </div>
      </header>

      <main className="app-main">
        <div className="cube-section">
          <div className="cube-container">
            <div className="cube-header">
              <h2>3D Cube</h2>
            </div>
            <div className="cube-display">
              <RubiksCube ref={cubeRef} cubeState={cubeState} />
            </div>
          </div>
        </div>

        <div className="controls-section">
          <div className="scramble-section">
            <div className="section-header">
              <h2>Scramble</h2>
            </div>
            <div className="scramble-controls">
              <textarea
                value={scrambleInput}
                onChange={(e) => setScrambleInput(e.target.value)}
                placeholder="Enter scramble (e.g., R U R' F R F') or generate random..."
                className="scramble-input"
                rows={3}
              />
              <div className="scramble-buttons">
                <button 
                  onClick={handleRandomScramble}
                  className="btn btn-outline"
                  disabled={isLoading}
                >
                  Random Scramble
                </button>
                <button 
                  onClick={handleApplyScramble}
                  className="btn btn-primary"
                  disabled={isLoading || !scrambleInput.trim()}
                >
                  Apply Scramble
                </button>
              </div>
            </div>
          </div>

          <div className="solver-section">
            <div className="section-header">
              <h2>Solver</h2>
            </div>
            <div className="solver-controls">
              <button 
                onClick={handleSolveCube}
                className="btn btn-success"
                disabled={isLoading || !currentScramble}
              >
                {isLoading ? 'Solving...' : 'Solve Cube'}
              </button>
            </div>
            
            {solution && (
              <div className="solution-display">
                <h3>Solution:</h3>
                <div className="solution-text">{solution}</div>
                <button 
                  onClick={() => {
                    if (cubeRef.current) {
                      // Parse solution string into array of moves
                      const moves = solution.trim().split(/\s+/).filter(m => m.length > 0);
                      // Pass both moves and the current cube state
                      cubeRef.current.animateSolution(moves, cubeState);
                    }
                  }}
                  className="btn btn-primary"
                  disabled={isLoading}
                >
                  Apply Solution
                </button>
              </div>
            )}
            <button 
              onClick={handleReset}
              className="btn btn-secondary"
              disabled={isLoading}
            >
              Reset
            </button>
          </div>
        </div>
      </main>
    </div>
  );
}

export default App;


