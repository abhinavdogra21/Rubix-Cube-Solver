
# 3D Interactive Rubik's Cube Solver

## Quick Start: Manual Configuration & Scramble Workflows

This project supports two main workflows:

1. **Manual Configuration**
   - Configure the cube in the frontend (enter colors/digits)
   - Click "Solve" (calls `/api/solve`)

2. **Scramble & Solve**
   - Click "Scramble" (calls `/api/scramble`)
   - Click "Solve" (calls `/api/solve` with the scrambled state)

Both workflows are fully supported by the backend and frontend. The backend automatically converts digit-based cube states to the letter format required by the solver.

### API Endpoints
- `/api/solve` (POST): Solve a cube from a cube state string (digits 0-5 or letters U,R,F,D,L,B)
- `/api/scramble` (GET): Get a random scramble and the corresponding scrambled cube state
- <video src="https://github.com/user-attachments/assets/c70d1f03-177a-4da5-a45c-88777340712f" controls width="700">
  Your browser does not support the video tag.
</video>

npm install
npm run dev

### How to Run Everything (macOS/Linux)


#### 1. Clean up all cache and build artifacts (recommended before every fresh build):
```sh
rm -rf cache
rm -rf backend/build
rm -rf backend/kociemba_api/build
rm -rf backend/kociemba_api/src/solver/cache
find . -name "kociemba_solver*.so" -delete
```


#### 2. Build and run the backend (C++/Python module):
```sh
cd backend
rm -rf build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
# Copy the built Python module to where main.py expects it
cp kociemba_solver*.so ../kociemba_api/src/
cd ../kociemba_api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
cd src
python main.py
```
Backend API: http://localhost:5001

> **Note:** The only CMakeLists.txt you need for backend build is in `backend/`. Ignore or delete any CMakeLists.txt or Makefile in `backend/src` or other subfolders.

#### 3. Start the frontend:
```sh
cd ../../../
npm install
npm run dev
```
Frontend: http://localhost:5173

### Example API Usage
Manual configuration:
```js
fetch('http://localhost:5001/api/solve', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ cube_state: '000000000111111111222222222333333333444444444555555555' })
})
  .then(res => res.json())
  .then(data => console.log(data.solution));
```

Scramble workflow:
```js
fetch('http://localhost:5001/api/scramble')
  .then(res => res.json())
  .then(data => {
    console.log(data.scramble); // scramble moves
    console.log(data.cube_state); // scrambled cube state
    // Now you can send data.cube_state to /api/solve
  });
```

---


## ✨ Features
- **3D Interactive Cube Visualization**: Real-time 3D rendering with smooth animations
- **Optimal Kociemba Algorithm (C++ backend)**: Fast, optimal solutions
- **Random Scramble Generation**: Generate and apply realistic scrambles
- **Step-by-Step Solution Animation**: Visualize and step through the solution
- **Manual Cube Configuration**: Set up any cube state with validation
- **Video Detection**: (Experimental) Detect cube state from your camera
- **Cross-platform**: Works on macOS and Windows

## Backend API Endpoints
- `/api/solve` (POST): Solve a cube from a cube state string (digits 0-5 or letters U,R,F,D,L,B)
- `/api/scramble` (GET): Get a random scramble and the corresponding scrambled cube state

## How to Run (macOS)

### Prerequisites
- Node.js 16+
- Python 3.8+
- CMake 3.12+
- C++ Compiler (GCC/Clang/MSVC)
- pybind11 (install via Homebrew or pip)

### Build & Run Backend
```sh
cd backend/kociemba_api
rm -rf build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
cp kociemba_solver*.so ../src/kociemba_solver.so
cd ..
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python src/main.py
```
Backend API: http://localhost:5001

### Build & Run Frontend
```sh
cd ../../..
# (from project root)
npm install
npm run dev
```
Frontend: http://localhost:5173

## Usage
- **Manual Configuration:**
  - Configure the cube in the frontend (enter colors/digits)
  - Click "Solve" (calls `/api/solve`)
- **Scramble & Solve:**
  - Click "Scramble" (calls `/api/scramble`)
  - Click "Solve" (calls `/api/solve` with scrambled state)

## Troubleshooting
- If you see CMake or Python errors, ensure correct Python version and headers.
- Do not commit build artifacts or .so files to GitHub.
- See `.gitignore` for details.

## Notes
- The backend supports both manual configuration and scramble workflows.
- Only `/api/solve` and `/api/scramble` endpoints are required for frontend integration.
- The backend does not animate or simulate moves; frontend handles visualization.


## ⚠️ Before You Start: Clean Clone & Common Build Issues

**1. Always clone a clean repo:**
- This repository should NOT include any prebuilt `.so` files or `build/` directories. If you see them, delete them before building.
- After cloning, always build the backend locally. Do not rely on any pre-existing build artifacts.

**2. Install pybind11:**
- On macOS (recommended):
  ```sh
  brew install pybind11
  ```
- Or with pip (if you use a Python virtualenv):
  ```sh
  pip install pybind11
  ```

**3. Standard backend build steps after cloning:**
```sh
cd backend/kociemba_api
rm -rf build
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
cp kociemba_solver*.so ../src/kociemba_solver.so
cd ..
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python src/main.py
```

**4. If you see CMake or Python errors:**
- Make sure you have the correct Python version and development headers (Homebrew Python is recommended on macOS).
- If you see errors about `FindPythonInterp` or `FindPythonLibs`, ensure your `CMakeLists.txt` contains:
  ```cmake
  set(PYBIND11_FINDPYTHON ON)
  find_package(pybind11 REQUIRED)
  ```
- If you see errors about `strip` or shell syntax, add this to your `CMakeLists.txt`:
  ```cmake
  set(CMAKE_STRIP "")
  ```
- Avoid spaces or parentheses in your project path if possible. If you can't, the above `CMAKE_STRIP` workaround is required.

**5. Do NOT commit these files to GitHub:**
- All `build/` directories
- All `.so`, `.pyd`, `.dll` files
- All `__pycache__/`, `.pyc`, `.egg-info/`, `venv/`, `.env/`, `node_modules/`
- (See `.gitignore` for details)

---

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

---

## 🛠️ How to Run the Backend for the First Time (and Fix pybind11/Python CMake Issues)

If you are running the backend for the first time, or encounter errors like:
- `CMake Error: The source ... does not match the source ... used to generate cache. Re-run cmake with a different source directory.`
- `Python config failure` or warnings about `FindPythonInterp`/`FindPythonLibs`

Follow these steps:

### 1. Clean the Build Directory
If you see a CMake cache/source mismatch error, run:
```sh
cd backend
rm -rf build
mkdir build && cd build
```

### 2. Fix pybind11/Python CMake Errors
If you see errors about Python config or pybind11, edit your `CMakeLists.txt` (in `backend/` or `backend/kociemba_api/`) and add this **before** `find_package(pybind11 REQUIRED)`:
```cmake
set(PYBIND11_FINDPYTHON ON)
find_package(pybind11 REQUIRED)
```
This uses the modern CMake Python finder and avoids deprecated warnings.

### 3. Build the Backend
```sh
cmake ..
make -j$(sysctl -n hw.ncpu)
cp kociemba_solver*.so ../kociemba_api/src/kociemba_solver.so
```

### 4. Set Up Python Environment
```sh
cd ../kociemba_api
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
python src/main.py
```

The backend should now run at http://localhost:5001

If you have further issues, see the Troubleshooting section above or open an issue.

---

### Frontend/Backend Integration

Your frontend should send the cube state as a string of 54 digits (0-5), where each digit represents a color:

- 0: U (Up)
- 1: R (Right)
- 2: F (Front)
- 3: D (Down)
- 4: L (Left)
- 5: B (Back)

The backend will automatically convert this digit string to the letter format required by the solver. You do not need to convert it in the frontend.

Example API call:
```js
fetch('http://localhost:5001/api/solve', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ cube_state: '000000000111111111222222222333333333444444444555555555' })
})
  .then(res => res.json())
  .then(data => console.log(data.solution));
```



