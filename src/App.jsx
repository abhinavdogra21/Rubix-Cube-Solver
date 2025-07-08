import React, { useState, useEffect, useRef } from 'react';
import RubiksCube from './RubiksCube';
import { CubeState } from './cubeState';
import CubeConfigurator from './CubeConfigurator';
import VideoInput from './VideoInput';
import './App.css';

const API_BASE_URL = 'https://rubix-cube-solver.onrender.com'; // Use Render backend

function App() {
  const [scrambleInput, setScrambleInput] = useState('');
  const [currentScramble, setCurrentScramble] = useState('');
  const [solution, setSolution] = useState('');
  const [cubeState, setCubeStateValue] = useState(new CubeState());
  const [isLoading, setIsLoading] = useState(false);
  const [apiStatus, setApiStatus] = useState('checking');
  const cubeRef = useRef();
  const [currentMoveIndex, setCurrentMoveIndex] = useState(-1);
  const [isAnimating, setIsAnimating] = useState(false);
  const [solutionMoves, setSolutionMoves] = useState([]);
  const [moveProgress, setMoveProgress] = useState({ current: 0, total: 0 });

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

  const generateRandomScramble = async () => {
    setIsLoading(true);
    try {
      const response = await fetch(`${API_BASE_URL}/api/scramble`);
      if (response.ok) {
        const data = await response.json();
        if (data.success) {
          setScrambleInput(data.scramble);
          setCurrentScramble(data.scramble);
          // Update cube state if provided
          if (data.cube_state) {
            const newCubeState = new CubeState();
            newCubeState.fromString(data.cube_state);
            setCubeStateValue(newCubeState);
          }
        } else {
          console.error('Scramble generation failed:', data.error);
          // Fallback to client-side generation
          generateFallbackScramble();
        }
      } else {
        generateFallbackScramble();
      }
    } catch (error) {
      console.error('Error generating scramble:', error);
      generateFallbackScramble();
    } finally {
      setIsLoading(false);
    }
  };

  const generateFallbackScramble = () => {
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
    
    const scrambleString = scramble.join(' ');
    setScrambleInput(scrambleString);
    setCurrentScramble(scrambleString);
  };

  const applyScramble = () => {
    if (!scrambleInput.trim()) return;
    
    const newCubeState = new CubeState();
    const moves = scrambleInput.trim().split(/\s+/);
    
    for (const move of moves) {
      newCubeState.applyMove(move);
    }
    
    setCubeStateValue(newCubeState);
    setCurrentScramble(scrambleInput);
    setSolution('');
    setSolutionMoves([]);
    setCurrentMoveIndex(-1);
    setMoveProgress({ current: 0, total: 0 });
  };

  const solveCube = async () => {
    if (!cubeState) return;
    
    setIsLoading(true);
    setSolution('');
    
    try {
      const cubeString = cubeState.toString();
      console.log('Sending cube state to solver:', cubeString);
      
      const response = await fetch(`${API_BASE_URL}/api/solve`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ cube_state: cubeString }),
      });
      
      if (response.ok) {
        const data = await response.json();
        if (data.success) {
          const solutionString = data.solution;
          setSolution(solutionString);
          
          if (solutionString && solutionString.trim()) {
            const moves = solutionString.trim().split(/\s+/);
            setSolutionMoves(moves);
            setMoveProgress({ current: 0, total: moves.length });
          }
        } else {
          setSolution(`Error: ${data.error}`);
          console.error('Solve failed:', data.error);
        }
      } else {
        setSolution('Error: Failed to connect to solver');
      }
    } catch (error) {
      console.error('Error solving cube:', error);
      setSolution('Error: Network error');
    } finally {
      setIsLoading(false);
    }
  };

  const resetCube = () => {
    const newCubeState = new CubeState();
    setCubeStateValue(newCubeState);
    setSolution('');
    setSolutionMoves([]);
    setCurrentMoveIndex(-1);
    setCurrentScramble('');
    setScrambleInput('');
    setMoveProgress({ current: 0, total: 0 });
  };

  const setCubeState = (newState) => {
    setCubeStateValue(newState);
    setSolution('');
    setSolutionMoves([]);
    setCurrentMoveIndex(-1);
    setMoveProgress({ current: 0, total: 0 });
  };

  const animateSolution = () => {
    if (solutionMoves.length === 0 || isAnimating) return;
    
    setIsAnimating(true);
    setCurrentMoveIndex(0);
    
    const animateMove = (index) => {
      if (index >= solutionMoves.length) {
        setIsAnimating(false);
        setCurrentMoveIndex(-1);
        return;
      }
      
      const move = solutionMoves[index];
      const newCubeState = new CubeState(cubeState);
      newCubeState.applyMove(move);
      setCubeStateValue(newCubeState);
      setCurrentMoveIndex(index);
      setMoveProgress({ current: index + 1, total: solutionMoves.length });
      
      setTimeout(() => animateMove(index + 1), 800);
    };
    
    animateMove(0);
  };

  const stepThroughSolution = (direction) => {
    if (solutionMoves.length === 0) return;
    
    let newIndex;
    if (direction === 'next') {
      newIndex = Math.min(currentMoveIndex + 1, solutionMoves.length - 1);
    } else {
      newIndex = Math.max(currentMoveIndex - 1, -1);
    }
    
    if (newIndex === currentMoveIndex) return;
    
    // Apply moves up to the new index
    const newCubeState = new CubeState();
    const moves = currentScramble.trim().split(/\s+/);
    
    // Apply scramble first
    for (const move of moves) {
      if (move.trim()) newCubeState.applyMove(move);
    }
    
    // Apply solution moves up to new index
    for (let i = 0; i <= newIndex; i++) {
      newCubeState.applyMove(solutionMoves[i]);
    }
    
    setCubeStateValue(newCubeState);
    setCurrentMoveIndex(newIndex);
    setMoveProgress({ current: newIndex + 1, total: solutionMoves.length });
  };

  const getApiStatusColor = () => {
    switch (apiStatus) {
      case 'available': return '#4CAF50';
      case 'limited': return '#FF9800';
      case 'unavailable': return '#F44336';
      default: return '#9E9E9E';
    }
  };

  const getApiStatusText = () => {
    switch (apiStatus) {
      case 'available': return 'In-house Solver Available';
      case 'limited': return 'Limited Functionality';
      case 'unavailable': return 'Backend Unavailable';
      default: return 'Checking...';
    }
  };

  return (
    <div className="App">
      <header className="App-header">
        <h1>🧩 3D Interactive Rubik's Cube Solver</h1>
        <div className="api-status" style={{ color: getApiStatusColor() }}>
          ● {getApiStatusText()}
        </div>
      </header>

      <main className="main-content">
        <div className="cube-container">
          <RubiksCube 
            ref={cubeRef}
            cubeState={cubeState} 
            onCubeStateChange={setCubeState}
          />
        </div>

        <div className="controls-container">
          <div className="control-section">
            <h3>🎲 Scramble</h3>
            <div className="scramble-controls">
              <input
                type="text"
                value={scrambleInput}
                onChange={(e) => setScrambleInput(e.target.value)}
                placeholder="Enter scramble (e.g., R U R' U')"
                className="scramble-input"
              />
              <div className="button-group">
                <button onClick={generateRandomScramble} disabled={isLoading}>
                  🎲 Generate Random
                </button>
                <button onClick={applyScramble} disabled={!scrambleInput.trim()}>
                  ✅ Apply Scramble
                </button>
              </div>
            </div>
            {currentScramble && (
              <div className="current-scramble">
                <strong>Current Scramble:</strong> {currentScramble}
              </div>
            )}
          </div>

          <div className="control-section">
            <h3>🔧 Solve</h3>
            <div className="solve-controls">
              <button 
                onClick={solveCube} 
                disabled={isLoading || cubeState?.isSolved()}
                className="solve-button"
              >
                {isLoading ? '🔄 Solving...' : '🧠 Find Solution'}
              </button>
              <button onClick={resetCube} className="reset-button">
                🔄 Reset Cube
              </button>
            </div>
            
            {solution && (
              <div className="solution-container">
                <div className="solution-header">
                  <strong>Solution:</strong>
                  {solutionMoves.length > 0 && (
                    <span className="move-counter">
                      ({moveProgress.current}/{moveProgress.total} moves)
                    </span>
                  )}
                </div>
                <div className="solution-text">{solution}</div>
                
                {solutionMoves.length > 0 && (
                  <div className="solution-controls">
                    <button 
                      onClick={animateSolution} 
                      disabled={isAnimating}
                      className="animate-button"
                    >
                      {isAnimating ? '⏸️ Animating...' : '▶️ Animate Solution'}
                    </button>
                    <div className="step-controls">
                      <button 
                        onClick={() => stepThroughSolution('prev')}
                        disabled={currentMoveIndex <= -1}
                      >
                        ⏮️ Previous
                      </button>
                      <button 
                        onClick={() => stepThroughSolution('next')}
                        disabled={currentMoveIndex >= solutionMoves.length - 1}
                      >
                        ⏭️ Next
                      </button>
                    </div>
                  </div>
                )}
              </div>
            )}
          </div>

          <div className="control-section">
            <h3>⚙️ Manual Configuration</h3>
            <CubeConfigurator cubeState={cubeState} onCubeStateChange={setCubeState} />
          </div>

          <div className="control-section">
            <h3>📹 Video Detection (Experimental)</h3>
            <VideoInput onCubeStateDetected={setCubeState} />
          </div>
        </div>
      </main>

      <footer className="App-footer">
        <p>
          Built with ❤️ using React + Three.js + In-house Kociemba Algorithm
        </p>
        <p>
          <a href="https://github.com/abhinavdogra21/Rubix-Cube-Solver" target="_blank" rel="noopener noreferrer">
            🔗 View Source Code
          </a>
        </p>
      </footer>
    </div>
  );
}

export default App;

