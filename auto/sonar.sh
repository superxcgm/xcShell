#!/bin/bash

set -e

mkdir build
cd build
conan install ..
cmake .. -DCMAKE_CXX_FLAGS="--coverage"
make
ctest --output-on-failure || true
cd ..
./auto/functional_test.sh run || true
cd build
gcovr --root .. . --exclude build --sonarqube > ../test-coverage.xml