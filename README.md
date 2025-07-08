

# 3D Interactive Rubik's Cube Solver

## ✨ Features

- **3D Interactive Cube Visualization**: Real-time 3D rendering with smooth animations
- **Optimal Kociemba Algorithm**: Fast, optimal solutions using a C++ backend
- **Random Scramble Generation**: Generate and apply realistic scrambles
- **Step-by-Step Solution Animation**: Visualize and step through the solution
- **Manual Cube Configuration**: Set up any cube state with validation
- **Video Detection**: (Experimental) Detect cube state from your camera
- **Cross-platform**: Works on macOS and Windows
  ![Image](https://github.com/user-attachments/assets/5d6bcfa8-ac16-4e69-9f44-cfa74a99ba6e)

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



