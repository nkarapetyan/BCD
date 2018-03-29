#!/bin/bash

cd "$(dirname "$0")"

[ -e Makefile ] && make clean
echo rm -f *.o
rm -f *.o
echo rm -f CMakeCache.txt Makefile cmake_install.cmake
rm -f CMakeCache.txt Makefile cmake_install.cmake
echo rm -rf CMakeFiles
rm -rf CMakeFiles

echo rm -f *.pro UseLibs
rm -f *.pro UseLibs

echo rm -f .qmake1.log .qmake2.log .qmake3.log .make.log .ldconfig.log
rm -f .qmake1.log .qmake2.log .qmake3.log .make.log .ldconfig.log
