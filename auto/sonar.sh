#!/bin/bash

set -e

mkdir build
cd build
conan install .. -s compiler.libcxx=libstdc++11
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
make
ctest --output-on-failure || true
cd ..
./auto/functional_test.sh run || true
cd build
gcovr --root .. . --exclude build --sonarqube > ../test-coverage.xml