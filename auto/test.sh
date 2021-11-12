#!/bin/bash

set -e

./auto/build.sh

cd build
ctest --output-on-failure