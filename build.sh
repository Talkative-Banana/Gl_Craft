#!/bin/bash

mkdir build
cd build
cmake ..
make -j4
cd ..
./GlCraft
./clean.sh
