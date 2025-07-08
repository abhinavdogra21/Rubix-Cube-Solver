#!/usr/bin/env bash
set -e

# Install Python dependencies
pip install -r backend/kociemba_api/requirements.txt

# Build the C++ extension
cd backend/kociemba_api

# Clean any existing build
rm -rf build/
rm -f src/kociemba_solver.so

# Build using setup.py
python3 setup.py build_ext --inplace

# Find the built .so file and copy/rename it to src/kociemba_solver.so
sofile=$(find . -name 'kociemba_solver*.so' | head -n 1)
if [ -n "$sofile" ]; then
  cp "$sofile" src/kociemba_solver.so
  echo "Successfully copied $sofile to src/kociemba_solver.so"
else
  echo "Warning: No .so file found, will use Python kociemba fallback"
fi

cd ../..


