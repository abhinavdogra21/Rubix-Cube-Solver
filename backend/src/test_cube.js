// Test script to debug cube solving
import { CubeState } from './src/lib/cubeState.js';

// Test the cube solving logic
function testCubeSolving() {
  console.log('Testing cube solving logic...');
  
  // Create a new cube (solved state)
  const cube = new CubeState();
  console.log('Initial cube state (should be solved):', cube.isSolved());
  
  // Apply a simple scramble
  const scramble = "R U R' F R F'";
  console.log('Applying scramble:', scramble);
  cube.applyMoves(scramble);
  console.log('Cube state after scramble (should be false):', cube.isSolved());
  
  // Apply the solution from the API
  const solution = "F R' F' R U' R' R U R' F R F' U R U' R' U' R U R'";
  console.log('Applying solution:', solution);
  cube.applyMoves(solution);
  console.log('Cube state after solution (should be true):', cube.isSolved());
  
  // Test individual moves
  const testCube = new CubeState();
  console.log('Testing individual moves...');
  testCube.applyMove('R');
  console.log('After R move, solved?', testCube.isSolved());
  testCube.applyMove("R'");
  console.log('After R\' move, solved?', testCube.isSolved());
}

testCubeSolving();

