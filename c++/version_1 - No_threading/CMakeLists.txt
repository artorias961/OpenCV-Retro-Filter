cmake_minimum_required(VERSION 4.2.1)

# Project name
project(OpenCVExample)

# Force compilers (ONLY if CMake cannot detect them)
#set(CMAKE_C_COMPILER gcc)
#set(CMAKE_CXX_COMPILER g++)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories from OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Add your source file(s)
add_executable(OpenCVExample main.cpp)

# Link OpenCV libraries
target_link_libraries(OpenCVExample ${OpenCV_LIBS})