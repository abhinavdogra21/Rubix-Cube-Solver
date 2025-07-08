# 3D Interactive Rubik's Cube Solver

A web-based Rubik's Cube solver with 3D visualization and a C++ Kociemba algorithm backend.

## ✨ Features

- **3D Interactive Cube Visualization**: Real-time 3D rendering with smooth animations
- **Optimal Kociemba Algorithm**: Fast, optimal solutions using a C++ backend
- **Random Scramble Generation**: Generate and apply realistic scrambles
- **Step-by-Step Solution Animation**: Visualize and step through the solution
- **Manual Cube Configuration**: Set up any cube state with validation
- **Video Detection**: (Experimental) Detect cube state from your camera
- **Cross-platform**: Works on macOS and Windows

## 🚀 Quick Start

### Prerequisites
- **Node.js** 16+ and npm
- **Python** 3.8+
- **CMake** 3.12+
- **C++ Compiler** (GCC, Clang, or MSVC)

## 🔧 Platform-Specific Setup

### macOS Setup

#### Prerequisites
```sh
xcode-select --install
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
brew install cmake python3 node
```

#### Build & Run
```sh
cd enhanced-rubiks-cube
npm install

cd backend
mkdir -p build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
cp kociemba_solver*.so ../kociemba_api/src/kociemba_solver.so

cd ../kociemba_api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python src/main.py
```
Backend API: http://localhost:5001

In a new terminal:
```sh
cd enhanced-rubiks-cube
npm run dev
```
Frontend: http://localhost:5173

---

### Windows Setup

#### Prerequisites
- Visual Studio 2019+ with C++ tools
- Python 3.8+ from python.org
- Node.js 16+ from nodejs.org
- CMake from cmake.org

#### Build & Run
```cmd
cd enhanced-rubiks-cube
npm install

cd backend
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019"
cmake --build . --config Release
copy Release\kociemba_solver*.pyd ..\kociemba_api\src\kociemba_solver.so

cd ..\kociemba_api
python -m venv venv
venv\Scripts\activate
pip install -r requirements.txt
python src\main.py
```
Backend API: http://localhost:5001

In a new terminal:
```cmd
cd enhanced-rubiks-cube
npm run dev
```
Frontend: http://localhost:5173

---

## 🐛 Troubleshooting

- **Backend Not Starting**: Ensure Python 3.8+ is installed, port 5001 is free, and the C++ library is built and copied correctly.
- **Frontend Build Errors**: Remove node_modules and reinstall. Check Node.js version.
- **C++ Compilation Errors**: Ensure CMake and a compatible compiler are installed.
- **Library Loading Issues**: Ensure the .so/.dll/.pyd file is in the correct location and readable.

For more help, see comments in the code or open an issue.

## 📄 License

MIT License.

