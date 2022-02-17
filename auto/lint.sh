#!/bin/bash

set -ex

./auto/build.sh

cd build
cmake --build . --target lint