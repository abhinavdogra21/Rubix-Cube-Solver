// Simple test to debug cube solving
const fs = require('fs');

// Read the CubeState class
const cubeStateCode = fs.readFileSync('./src/lib/cubeState.js', 'utf8');

// Create a simple module system
const module = { exports: {} };
const exports = module.exports;

// Execute the code
eval(cubeStateCode.replace('export class', 'class'));

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
  
  // Test if the solution is actually correct
  console.log('\nTesting solution correctness...');
  const testCube2 = new CubeState();
  testCube2.applyMoves(scramble);
  console.log('After scramble, cube state:', testCube2.isSolved());
  
  // Try to reverse the scramble manually
  const reverseScramble = "F' R' F R' U' R";
  testCube2.applyMoves(reverseScramble);
  console.log('After reverse scramble, cube state:', testCube2.isSolved());
}

testCubeSolving();

