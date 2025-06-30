import React, { useRef, useEffect, useCallback } from 'react';
import * as THREE from 'three';
import { OrbitControls } from 'three/examples/jsm/controls/OrbitControls';
import { setCubeState, getCubeState, applyMove } from './cubeState';

const RubiksCube = React.forwardRef((props, ref) => {
  const mountRef = useRef(null);
  const cubeGroupRef = useRef(new THREE.Group());
  const cubesRef = useRef([]);
  const currentCubeStateRef = useRef(getCubeState());

  const createCube = useCallback((x, y, z, faceColors) => {
    const geometry = new THREE.BoxGeometry(1, 1, 1);
    const materials = faceColors.map(color => new THREE.MeshBasicMaterial({ color }));
    const cube = new THREE.Mesh(geometry, materials);
    cube.position.set(x, y, z);
    return cube;
  }, []);

  const updateCubeColors = useCallback(() => {
    const state = getCubeState();
    const faceletColors = {
      'U': 0xFFFFFF, // White
      'R': 0xFF0000, // Red
      'F': 0x00FF00, // Green
      'D': 0xFFFF00, // Yellow
      'L': 0xFFA500, // Orange
      'B': 0x0000FF  // Blue
    };

    const faceletMap = {
      0: 'U', 1: 'U', 2: 'U', 3: 'U', 4: 'U', 5: 'U', 6: 'U', 7: 'U', 8: 'U',
      9: 'R', 10: 'R', 11: 'R', 12: 'R', 13: 'R', 14: 'R', 15: 'R', 16: 'R', 17: 'R',
      18: 'F', 19: 'F', 20: 'F', 21: 'F', 22: 'F', 23: 'F', 24: 'F', 25: 'F', 26: 'F',
      27: 'D', 28: 'D', 29: 'D', 30: 'D', 31: 'D', 32: 'D', 33: 'D', 34: 'D', 35: 'D',
      36: 'L', 37: 'L', 38: 'L', 39: 'L', 40: 'L', 41: 'L', 42: 'L', 43: 'L', 44: 'L',
      45: 'B', 46: 'B', 47: 'B', 48: 'B', 49: 'B', 50: 'B', 51: 'B', 52: 'B', 53: 'B'
    };

    const getFaceletColor = (faceletIndex) => {
      const colorChar = state[faceletIndex];
      return faceletColors[Object.keys(faceletColors)[parseInt(colorChar)]];
    };

    // Simplified color application for demonstration
    // This part would need a more complex mapping for actual facelets
    cubesRef.current.forEach((cube, index) => {
      const materials = cube.material;
      // Example: set a single color for all faces of a mini-cube
      // In a real implementation, you'd map facelets to specific mini-cube faces
      materials.forEach(mat => mat.color.set(0x808080)); // Default grey
    });

    // For now, just show the overall cube structure
  }, []);

  useEffect(() => {
    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth * 0.8, window.innerHeight * 0.8); // Adjust size
    mountRef.current.appendChild(renderer.domElement);

    camera.position.z = 5;

    const controls = new OrbitControls(camera, renderer.domElement);
    controls.enableDamping = true;
    controls.dampingFactor = 0.25;
    controls.screenSpacePanning = false;
    controls.maxPolarAngle = Math.PI / 2;

    // Create 27 mini-cubes (only 26 visible for a hollow cube)
    for (let x = -1; x <= 1; x++) {
      for (let y = -1; y <= 1; y++) {
        for (let z = -1; z <= 1; z++) {
          if (x === 0 && y === 0 && z === 0) continue; // Skip center
          const cube = createCube(x, y, z, [
            0x808080, 0x808080, 0x808080, // Right, Left, Top
            0x808080, 0x808080, 0x808080  // Bottom, Front, Back
          ]);
          cubeGroupRef.current.add(cube);
          cubesRef.current.push(cube);
        }
      }
    }
    scene.add(cubeGroupRef.current);

    const animate = () => {
      requestAnimationFrame(animate);
      controls.update();
      renderer.render(scene, camera);
    };
    animate();

    const handleResize = () => {
      camera.aspect = window.innerWidth / window.innerHeight;
      camera.updateProjectionMatrix();
      renderer.setSize(window.innerWidth * 0.8, window.innerHeight * 0.8);
    };
    window.addEventListener('resize', handleResize);

    const handleCubeStateChange = () => {
      updateCubeColors();
    };
    window.addEventListener('cubeStateChange', handleCubeStateChange);

    return () => {
      window.removeEventListener('resize', handleResize);
      window.removeEventListener('cubeStateChange', handleCubeStateChange);
      mountRef.current.removeChild(renderer.domElement);
      renderer.dispose();
    };
  }, [createCube, updateCubeColors]);

  // Expose methods to parent component via ref
  React.useImperativeHandle(ref, () => ({
    applyMoveToCube: (move) => {
      // This would involve animating the move in 3D
      // For now, just update the logical state
      applyMove(move);
    },
    updateVisualCube: () => {
      updateCubeColors();
    }
  }));

  return <div ref={mountRef} style={{ width: '100%', height: '100%' }} />;
});

export default RubiksCube;
