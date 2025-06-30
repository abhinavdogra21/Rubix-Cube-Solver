import React, { useState, useEffect, useRef } from 'react';
import RubiksCube from './RubiksCube';
import { scrambleCube, solveCube, getCubeState, setCubeState } from './cubeState';
import './App.css';

function App() {
  const [scramble, setScramble] = useState('');
  const [solution, setSolution] = useState('');
  const [cubeState, setLocalCubeState] = useState(getCubeState());
  const cubeRef = useRef();

  useEffect(() => {
    const handleCubeStateChange = () => {
      setLocalCubeState(getCubeState());
    };
    window.addEventListener('cubeStateChange', handleCubeStateChange);
    return () => {
      window.removeEventListener('cubeStateChange', handleCubeStateChange);
    };
  }, []);

  const handleScramble = () => {
    const newScramble = scrambleCube();
    setScramble(newScramble);
    setSolution('');
  };

  const handleSolve = () => {
    const currentCubeState = getCubeState();
    const sol = solveCube(currentCubeState);
    setSolution(sol);
  };

  const handleReset = () => {
    setCubeState('UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB'); // Solved state
    setScramble('');
    setSolution('');
  };

  return (
    <div className="App">
      <h1>3D Rubik's Cube Solver</h1>
      <div className="controls">
        <button onClick={handleScramble}>Scramble</button>
        <button onClick={handleSolve}>Solve</button>
        <button onClick={handleReset}>Reset</button>
      </div>
      {scramble && <p>Scramble: {scramble}</p>}
      {solution && <p>Solution: {solution}</p>}
      <div className="cube-container">
        <RubiksCube ref={cubeRef} />
      </div>
    </div>
  );
}

export default App;
