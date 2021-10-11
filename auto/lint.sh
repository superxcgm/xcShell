#!/bin/sh

set -e

./auto/build.sh

cd build
cmake --build . --target lint