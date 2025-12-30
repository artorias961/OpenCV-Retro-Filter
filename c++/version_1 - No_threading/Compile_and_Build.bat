@echo off
echo Cleaning build directory...
rmdir /S /Q build

echo Configuring project...
cmake -S . -B build -G "MinGW Makefiles" ^
 -DCMAKE_C_COMPILER=gcc ^
 -DCMAKE_CXX_COMPILER=g++

echo Building project...
cmake --build build

echo Running executable...
build\OpenCVExample.exe

pause
