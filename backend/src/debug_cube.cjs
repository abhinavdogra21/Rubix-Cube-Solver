// Simple test to debug cube solving
const fs = require('fs');

// Read the CubeState class
const cubeStateCode = fs.readFileSync('./src/lib/cubeState.js', 'utf8');

// Execute the code by creating a simple eval context
eval(cubeStateCode.replace('export class', 'global.CubeState = class'));

// Test the cube solving logic
function testCubeSolving() {
  console.log('Testing cube solving logic...');
  
  // Create a new cube (solved state)
  const cube = new global.CubeState();
  console.log('Initial cube state (should be solved):', cube.isSolved());
  
  // Apply a simple scramble
  const scramble = "R U R' F R F'";
  console.log('Applying scramble:', scramble);
  cube.applyMoves(scramble);
  console.log('Cube state after scramble (should be false):', cube.isSolved());
  
  // Apply the solution from the API
  const solution = "F R' F' R U' R'";
  console.log('Applying solution:', solution);
  cube.applyMoves(solution);
  console.log('Cube state after solution (should be true):', cube.isSolved());
  
  // Test individual moves
  const testCube = new global.CubeState();
  console.log('Testing individual moves...');
  testCube.applyMove('R');
  console.log('After R move, solved?', testCube.isSolved());
  testCube.applyMove("R'");
  console.log('After R\' move, solved?', testCube.isSolved());
  
  // Test if the solution is actually correct
  console.log('\nTesting solution correctness...');
  const testCube2 = new global.CubeState();
  testCube2.applyMoves(scramble);
  console.log('After scramble, cube state:', testCube2.isSolved());
  
  // Try to reverse the scramble manually
  const reverseScramble = "F' R' F R' U' R";
  testCube2.applyMoves(reverseScramble);
  console.log('After reverse scramble, cube state:', testCube2.isSolved());
}

testCubeSolving();

