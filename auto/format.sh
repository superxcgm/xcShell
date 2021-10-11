#!/bin/bash

set -e

./auto/build.sh

cd build
cmake --build . --target format