import React, { useState, useRef, useCallback } from 'react';
import { createPortal } from 'react-dom';
import { CubeState } from './cubeState';
import './VideoInput.css';

// Helper to open a popup window
function openPopupWindow(url, title, w, h) {
  // Center the popup
  const dualScreenLeft = window.screenLeft !== undefined ? window.screenLeft : window.screenX;
  const dualScreenTop = window.screenTop !== undefined ? window.screenTop : window.screenY;
  const width = window.innerWidth ? window.innerWidth : document.documentElement.clientWidth ? document.documentElement.clientWidth : screen.width;
  const height = window.innerHeight ? window.innerHeight : document.documentElement.clientHeight ? document.documentElement.clientHeight : screen.height;
  const left = ((width / 2) - (w / 2)) + dualScreenLeft;
  const top = ((height / 2) - (h / 2)) + dualScreenTop;
  const popup = window.open(url, title, `scrollbars=yes, width=${w}, height=${h}, top=${top}, left=${left}`);
  return popup;
}

// Helper: convert RGB to HSV
function rgbToHsv(r, g, b) {
  r /= 255; g /= 255; b /= 255;
  const max = Math.max(r, g, b), min = Math.min(r, g, b);
  let h, s, v = max;
  const d = max - min;
  s = max === 0 ? 0 : d / max;
  if (max === min) {
    h = 0; // achromatic
  } else {
    switch (max) {
      case r: h = (g - b) / d + (g < b ? 6 : 0); break;
      case g: h = (b - r) / d + 2; break;
      case b: h = (r - g) / d + 4; break;
      default: h = 0;
    }
    h /= 6;
  }
  return [h * 360, s, v];
}

// Map HSV to cube color letter
function hsvToCubeColor(h, s, v) {
  // simple thresholds
  if (v > 0.9 && s < 0.2) return 'W'; // white
  if (v > 0.75 && h >= 50 && h <= 70) return 'Y'; // yellow
  if (s > 0.4 && h >= 100 && h <= 140) return 'G'; // green
  if (s > 0.4 && h >= 200 && h <= 260) return 'B'; // blue
  if (s > 0.4 && (h <= 10 || h >= 350)) return 'R'; // red
  if (s > 0.4 && h >= 20 && h <= 40) return 'O'; // orange
  return 'W'; // default
}

function cubeColorToHex(c){
  return {W:'#ffffff',Y:'#ffff00',G:'#00ff00',B:'#0000ff',R:'#ff0000',O:'#ff8000'}[c]||'rgba(255,255,255,0.1)';
}

function detectFaceColorsFromCanvas(canvas) {
  const ctx = canvas.getContext('2d');
  const { width, height } = canvas;
  const cellW = width / 3;
  const cellH = height / 3;
  const colors = [];
  for (let row = 0; row < 3; row++) {
    for (let col = 0; col < 3; col++) {
      const x = Math.floor(col * cellW + cellW / 2);
      const y = Math.floor(row * cellH + cellH / 2);
      const data = ctx.getImageData(x - 4, y - 4, 8, 8).data;
      let rSum = 0, gSum = 0, bSum = 0, count = 0;
      for (let i = 0; i < data.length; i += 4) {
        rSum += data[i];
        gSum += data[i + 1];
        bSum += data[i + 2];
        count++;
      }
      const r = rSum / count;
      const g = gSum / count;
      const b = bSum / count;
      const [h, s, v] = rgbToHsv(r, g, b);
      colors.push(hsvToCubeColor(h, s, v));
    }
  }
  return colors;
}

export default function VideoInput({ onCubeDetected }) {
  const [isOpen, setIsOpen] = useState(false);
  const [isProcessing, setIsProcessing] = useState(false);
  const [currentFace, setCurrentFace] = useState('front');
  const [capturedFaces, setCapturedFaces] = useState({ front:null,back:null,left:null,right:null,top:null,bottom:null });
  const videoRef = useRef(null);
  const canvasRef = useRef(null);
  const streamRef = useRef(null);

  const faceOrder = ['front','right','back','left','top','bottom'];
  const faceInstructions = {
    front: 'Show the front face (white center)',
    right: 'Show the right face (blue center)',
    back:  'Show the back face (yellow center)',
    left:  'Show the left face (green center)',
    top:   'Show the top face (red center)',
    bottom:'Show the bottom face (orange center)'
  };

  const startCamera = async () => {
    if (isOpen) return;
    // attempt environment -> user -> any camera
    const constraintsList = [
      { video: { width: 800, height: 480, facingMode: { ideal: 'environment' } }, audio: false },
      { video: { width: 800, height: 480, facingMode: 'user' }, audio: false },
      { video: true }
    ];
    let stream;
    for (const c of constraintsList) {
      try { stream = await navigator.mediaDevices.getUserMedia(c); break; } catch {}
    }
    if (!stream) {
      alert('Error accessing camera. Please ensure camera permissions are granted.');
      return;
    }
    streamRef.current = stream;
    setIsOpen(true);
    // give React time to render dialog, then attach stream
    setTimeout(()=>{
      if (videoRef.current) {
        videoRef.current.srcObject = stream;
        videoRef.current.play();
      }
    },0);
  };

  const stopCamera = () => {
    if (streamRef.current) {
      streamRef.current.getTracks().forEach(t=>t.stop());
      streamRef.current = null;
    }
    setIsOpen(false);
    setIsProcessing(false);
    setCurrentFace('front');
    setCapturedFaces({ front:null,back:null,left:null,right:null,top:null,bottom:null });
  };

  const handleCapture = async () => {
    if (!videoRef.current || isProcessing) return;
    
      setIsProcessing(true);
      try {
        const canvas = canvasRef.current;
      canvas.width = videoRef.current.videoWidth;
      canvas.height = videoRef.current.videoHeight;
        const ctx = canvas.getContext('2d');
      ctx.drawImage(videoRef.current, 0, 0);
      const detected = detectFaceColorsFromCanvas(canvas);
      const newFaces = { ...capturedFaces, [currentFace]: detected };
      setCapturedFaces(newFaces);
      
      const idx = faceOrder.indexOf(currentFace);
      if (idx === faceOrder.length - 1) {
        // all faces captured
        const cube = new CubeState();
        Object.entries(newFaces).forEach(([f, arr]) => { cube.faces[f] = arr; });
        if (onCubeDetected) onCubeDetected(cube);
        stopCamera();
        return;
      }
      
      const nextFace = faceOrder[(idx + 1) % faceOrder.length];
      setCurrentFace(nextFace);
      } catch (error) {
      console.error('Error capturing face:', error);
      alert('Failed to capture face. Please try again.');
      } finally {
        setIsProcessing(false);
      }
  };

  // modal markup
  const modal = isOpen ? createPortal(
    <div className="video-modal" role="dialog">
      <div className="video-container">
        <video ref={videoRef} className="video-preview" muted playsInline></video>
        <div className="grid-overlay">
          {Array.from({length:9}).map((_,i)=>{
            const faceColors = capturedFaces[currentFace];
            const bg = faceColors ? cubeColorToHex(faceColors[i]) : 'rgba(0,0,0,0)';
            return <div key={i} className={`grid-cell ${i===4?'center':''}`} style={{background:bg,opacity:faceColors?0.8:0}}/>;
          })}
            </div>
        <div className="instructions">{faceInstructions[currentFace]}</div>
          </div>
      <canvas ref={canvasRef} style={{display:'none'}} />
      <div className="face-progress">
        {faceOrder.map(f=>(<div key={f} className={`face-indicator ${capturedFaces[f]?'captured':''} ${f===currentFace?'current':''}`}>{f[0].toUpperCase()}</div>))}
      </div>
      <div>
        <button className="btn btn-capture" disabled={isProcessing} onClick={handleCapture}>📸 Capture Face</button>
        <button className="btn btn-stop" onClick={stopCamera}>❌ Stop Camera</button>
      </div>
    </div>, document.body) : null;

  return (
    <>
      <button className="btn btn-primary" onClick={startCamera}>Start Camera</button>
      {modal}
    </>
  );
}

