#!/bin/bash
rm -rf build
rm GlCraft
mkdir build
cd build
cmake ..
make -j 4
cd ..
./GlCraft
