#!/bin/bash

set -ex

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build .
ctest --output-on-failure || true
cd ..
./auto/functional_test.sh run || true
cd build
gcovr --root .. . --exclude build --sonarqube > ../test-coverage.xml