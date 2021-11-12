#!/bin/bash

cd build
conan install ..
cmake .. -DCMAKE_CXX_FLAGS="--coverage"
make
ctest --output-on-failure
cd ..
./auto/functional_test.sh run
cd build
gcovr --root .. . --exclude build --sonarqube > ../test-coverage.xml