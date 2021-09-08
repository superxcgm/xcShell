#!/bin/bash

set -e

mkdir -p build
cd build
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  conan install .. -s compiler.libcxx=libstdc++11
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
  conan install ..
else
  conan install ..
fi
cmake ..
# build
cmake --build .
# test
cmake --build . --target test