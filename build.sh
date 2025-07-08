#!/usr/bin/env bash
set -e

# Install system dependencies
apt-get update
apt-get install -y build-essential python3-dev

# Install Python dependencies
pip install -r backend/kociemba_api/requirements.txt

# Build the C++ extension
cd backend/kociemba_api
python3 setup.py build_ext --inplace

# Find the built .so file and copy/rename it to src/kociemba_solver.so
sofile=$(find . -name 'kociemba_solver*.so' | head -n 1)
if [ -n "$sofile" ]; then
  cp "$sofile" src/kociemba_solver.so
fi

cd ../..
