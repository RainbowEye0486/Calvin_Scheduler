#!/bin/sh
mkdir build & cd build
cmake ..
make
cd ..
./build/bin/scheduler