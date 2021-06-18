#!/bin/sh

set -e

mkdir -p build
cd build
conan install ..
cmake ..
# build
cmake --build .
# test
cmake --build . --target test