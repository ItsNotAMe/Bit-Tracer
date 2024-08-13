# Specify the minimum required CMake version
cmake_minimum_required(VERSION 3.19)

# Set the project name
project(BitTracer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Add the source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Include vendor folder
include_directories(vendor/stb_image)

# Add the executable
add_executable(BitTracer ${SOURCES})

# Include directories
target_include_directories(BitTracer PRIVATE src)

# Set the build directory
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})